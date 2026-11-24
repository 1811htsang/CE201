/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define I2S_SAMPLE_RATE        32000U
#define RECORDING_SECONDS      10U
#define RECORDING_SAMPLES      (I2S_SAMPLE_RATE * RECORDING_SECONDS)
#define I2S_DMA_SAMPLES        2048U
#define I2S_DMA_HALF_SAMPLES   (I2S_DMA_SAMPLES / 2U)
#define RING_BUF_SAMPLES       16384U  /* Phai la luy thua cua 2 (chua 512ms audio) */
#define WARMUP_HALF_BLOCKS     20U      /* Ghi ngay tu dau; khong bo sot tin hieu thu am */
#define UART_TX_TIMEOUT_MS     100U
#define GENERATE_CSV_FILE      1U      /* Dat 0 neu chi can .bin va .wav cho nhanh */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2S_HandleTypeDef hi2s1;
DMA_HandleTypeDef hdma_spi1_rx;

UART_HandleTypeDef hlpuart1;

SD_HandleTypeDef hsd1;

/* USER CODE BEGIN PV */
/* DMA1 can access D2 SRAM; the linker places this section at 0x30000000. */
__attribute__((section(".dma_buffer"), aligned(32)))
static uint32_t i2s_rx_buffer[I2S_DMA_SAMPLES];

static int32_t ring_buf[RING_BUF_SAMPLES];
static volatile uint32_t ring_head = 0U;
static volatile uint32_t ring_tail = 0U;
static volatile uint32_t warmup_left = 0U;
static volatile uint32_t samples_captured = 0U;
static volatile uint32_t i2s_dma_error = 0U;
static volatile uint32_t ring_overflow = 0U;
static volatile int32_t capture_peak = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2S1_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_SDMMC1_SD_Init(void);
/* USER CODE BEGIN PFP */
static void RecordI2S10Seconds(void);
static int32_t DecodeI2S24(uint32_t raw_sample);
static void PushI2SHalfToRing(const uint32_t *half_samples);
static void CreateWavAndCsvFromBin(const char *bin_path,
                                   const char *wav_path,
                                   const char *csv_path);
static void TransmitUartText(const char *text, uint16_t length);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static int32_t DecodeI2S24(uint32_t raw_sample)
{
  /* Data24BitAlignment = LEFT -> 24-bit nam o [31:8], dich phai giu dau */
  return (int32_t)raw_sample >> 8;
}

static void PushI2SHalfToRing(const uint32_t *half_samples)
{
  if (warmup_left > 0U)
  {
    warmup_left--;
    return;
  }

  for (uint32_t i = 0U; i < I2S_DMA_HALF_SAMPLES; i += 2U)
  {
    if (samples_captured >= RECORDING_SAMPLES)
    {
      break;
    }

    uint32_t next_head = (ring_head + 1U) & (RING_BUF_SAMPLES - 1U);
    if (next_head == ring_tail)
    {
      ring_overflow = 1U;
      break;
    }

    int32_t sample = DecodeI2S24(half_samples[i]);
    ring_buf[ring_head] = sample; /* Kenh L (index chan) */
    int32_t magnitude = (sample < 0) ? -(sample + 1) + 1 : sample;
    if (magnitude > capture_peak)
    {
      capture_peak = magnitude;
    }
    __DMB();
    ring_head = next_head;
    samples_captured++;
  }
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if (hi2s == &hi2s1)
  {
    PushI2SHalfToRing(&i2s_rx_buffer[0]);
  }
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if (hi2s == &hi2s1)
  {
    PushI2SHalfToRing(&i2s_rx_buffer[I2S_DMA_HALF_SAMPLES]);
  }
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
  if (hi2s == &hi2s1)
  {
    __IO uint32_t tmpreg = hi2s->Instance->SR;
    (void)tmpreg;
    i2s_dma_error = 1U;
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2S1_Init();
  MX_LPUART1_UART_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  RecordI2S10Seconds();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 13;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 2560;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2S1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S1_Init(void)
{

  /* USER CODE BEGIN I2S1_Init 0 */

  /* USER CODE END I2S1_Init 0 */

  /* USER CODE BEGIN I2S1_Init 1 */

  /* USER CODE END I2S1_Init 1 */
  hi2s1.Instance = SPI1;
  hi2s1.Init.Mode = I2S_MODE_MASTER_RX;
  hi2s1.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s1.Init.DataFormat = I2S_DATAFORMAT_24B;
  hi2s1.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_32K;
  hi2s1.Init.CPOL = I2S_CPOL_LOW;
  hi2s1.Init.FirstBit = I2S_FIRSTBIT_MSB;
  hi2s1.Init.WSInversion = I2S_WS_INVERSION_DISABLE;
  hi2s1.Init.Data24BitAlignment = I2S_DATA_24BIT_ALIGNMENT_LEFT;
  hi2s1.Init.MasterKeepIOState = I2S_MASTER_KEEP_IO_STATE_DISABLE;
  if (HAL_I2S_Init(&hi2s1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S1_Init 2 */

  /* USER CODE END I2S1_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 921600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 8;
  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static void TransmitUartText(const char *text, uint16_t length)
{
  (void)HAL_UART_Transmit(&hlpuart1, (const uint8_t *)text, length, UART_TX_TIMEOUT_MS);
}

static void WriteLittleEndian32(uint8_t *dst, uint32_t val)
{
  dst[0] = (uint8_t)(val & 0xFFU);
  dst[1] = (uint8_t)((val >> 8) & 0xFFU);
  dst[2] = (uint8_t)((val >> 16) & 0xFFU);
  dst[3] = (uint8_t)((val >> 24) & 0xFFU);
}

static void WriteLittleEndian16(uint8_t *dst, uint16_t val)
{
  dst[0] = (uint8_t)(val & 0xFFU);
  dst[1] = (uint8_t)((val >> 8) & 0xFFU);
}

static void CreateWavAndCsvFromBin(const char *bin_path,
                                   const char *wav_path,
                                   const char *csv_path)
{
  static FIL bin_file;
  static FIL out_file;
  __attribute__((section(".dma_buffer"), aligned(32))) static int32_t in_chunk[512];
  __attribute__((section(".dma_buffer"), aligned(32))) static int16_t wav_chunk[512];
  __attribute__((section(".dma_buffer"), aligned(32))) static char csv_chunk[4096];
  uint8_t wav_hdr[44];
  UINT br = 0U, bw = 0U;
  uint32_t data_bytes_16 = RECORDING_SAMPLES * 2U;

  /* 1. Tao file WAV 16-bit PCM (32kHz, Mono) de nghe truc tiep tren PC */
  memcpy(&wav_hdr[0],  "RIFF", 4);
  WriteLittleEndian32(&wav_hdr[4],  36U + data_bytes_16);
  memcpy(&wav_hdr[8],  "WAVE", 4);
  memcpy(&wav_hdr[12], "fmt ", 4);
  WriteLittleEndian32(&wav_hdr[16], 16U);              /* PCM */
  WriteLittleEndian16(&wav_hdr[20], 1U);               /* Format = 1 */
  WriteLittleEndian16(&wav_hdr[22], 1U);               /* 1 Kenh (Mono) */
  WriteLittleEndian32(&wav_hdr[24], I2S_SAMPLE_RATE);  /* 32000 Hz */
  WriteLittleEndian32(&wav_hdr[28], I2S_SAMPLE_RATE * 2U);
  WriteLittleEndian16(&wav_hdr[32], 2U);               /* BlockAlign */
  WriteLittleEndian16(&wav_hdr[34], 16U);              /* 16 bits/sample */
  memcpy(&wav_hdr[36], "data", 4);
  WriteLittleEndian32(&wav_hdr[40], data_bytes_16);

  if (f_open(&bin_file, bin_path, FA_READ) != FR_OK) Error_Handler();
  if (f_open(&out_file, wav_path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) Error_Handler();
  if ((f_write(&out_file, wav_hdr, 44U, &bw) != FR_OK) || (bw != 44U)) Error_Handler();

  while (1)
  {
    if (f_read(&bin_file, in_chunk, sizeof(in_chunk), &br) != FR_OK) Error_Handler();
    if (br == 0U) break;
    uint32_t n = br / sizeof(int32_t);
    for (uint32_t i = 0U; i < n; ++i)
    {
      /* Chuyen 24-bit co dau sang 16-bit WAV (dich bo 8 bit thap) */
      wav_chunk[i] = (int16_t)(in_chunk[i] >> 8);
    }
    if ((f_write(&out_file, wav_chunk, n * sizeof(int16_t), &bw) != FR_OK) ||
        (bw != n * sizeof(int16_t)))
    {
      Error_Handler();
    }
  }
  f_close(&out_file);
  f_close(&bin_file);

#if GENERATE_CSV_FILE
  /* 2. Xuat file CSV muot ma tu file BIN (khong anh huong DMA) */
  if (f_open(&bin_file, bin_path, FA_READ) != FR_OK) Error_Handler();
  if (f_open(&out_file, csv_path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) Error_Handler();

  static const char csv_hdr[] = "sample_index,sample_24bit\r\n";
  if ((f_write(&out_file, csv_hdr, sizeof(csv_hdr) - 1U, &bw) != FR_OK) ||
      (bw != sizeof(csv_hdr) - 1U))
  {
    Error_Handler();
  }

  uint32_t idx = 0U;
  while (1)
  {
    if (f_read(&bin_file, in_chunk, sizeof(in_chunk), &br) != FR_OK) Error_Handler();
    if (br == 0U) break;
    uint32_t n = br / sizeof(int32_t);
    UINT csv_len = 0U;
    for (uint32_t i = 0U; i < n; ++i)
    {
      int w = snprintf(&csv_chunk[csv_len], sizeof(csv_chunk) - csv_len,
                       "%lu,%ld\r\n", (unsigned long)idx++, (long)in_chunk[i]);
      if ((w < 0) || ((UINT)w >= (sizeof(csv_chunk) - csv_len))) Error_Handler();
      csv_len += (UINT)w;
      if (csv_len > (sizeof(csv_chunk) - 64U))
      {
        if ((f_write(&out_file, csv_chunk, csv_len, &bw) != FR_OK) || (bw != csv_len))
          Error_Handler();
        csv_len = 0U;
      }
    }
    if (csv_len > 0U)
    {
      if ((f_write(&out_file, csv_chunk, csv_len, &bw) != FR_OK) || (bw != csv_len))
        Error_Handler();
    }
  }
  f_close(&out_file);
  f_close(&bin_file);
#else
  (void)csv_path;
#endif
}

static void RecordI2S10Seconds(void)
{
  __attribute__((section(".dma_buffer"), aligned(32))) static int32_t sd_write_chunk[1024]; /* 4KB nam trong D2 SRAM, chuan DMA */
  char bin_path[32], wav_path[32], csv_path[32];
  uint32_t samples_written = 0U;
  UINT bw = 0U;

  if (retSD != 0U) Error_Handler();
  if (f_mount(&SDFatFS, (TCHAR const *)SDPath, 1U) != FR_OK) Error_Handler();

  (void)snprintf(bin_path, sizeof(bin_path), "%si2s_32k_10s.bin", SDPath);
  (void)snprintf(wav_path, sizeof(wav_path), "%si2s_32k_10s.wav", SDPath);
  (void)snprintf(csv_path, sizeof(csv_path), "%si2s_32k_10s.csv", SDPath);

  if (f_open(&SDFile, bin_path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) Error_Handler();

  const char msg_start[] = "\r\n=== Recording 10s (32kHz) to BIN ===\r\n";
  TransmitUartText(msg_start, (uint16_t)(sizeof(msg_start) - 1U));

//  memset(i2s_tx_silence, 0, sizeof(i2s_tx_silence));
  ring_head = 0U;
  ring_tail = 0U;
  samples_captured = 0U;
  warmup_left = WARMUP_HALF_BLOCKS;
  i2s_dma_error = 0U;
  ring_overflow = 0U;
  capture_peak = 0;

  /* SUA LOI QUAN TRONG: Truyen dung I2S_DMA_SAMPLES (KHONG nhan * 2U!) */
  if (HAL_I2S_Receive_DMA(&hi2s1,
                            (uint16_t *)i2s_rx_buffer,
							(uint16_t)(I2S_DMA_SAMPLES)) != HAL_OK)
    {
      Error_Handler();
    }

  while (samples_written < RECORDING_SAMPLES)
  {
    if ((i2s_dma_error != 0U) || (ring_overflow != 0U))
    {
      Error_Handler();
    }

    uint32_t head = ring_head;
    uint32_t avail = (head - ring_tail) & (RING_BUF_SAMPLES - 1U);


    /* Ghi theo tung cuc lon 1024 mau (4KB) hoac ghi not phan cuoi */
    if ((avail >= 1024U) || ((samples_captured >= RECORDING_SAMPLES) && (avail > 0U)))
	{
	  uint32_t n = (avail > 1024U) ? 1024U : avail;
	  for (uint32_t i = 0U; i < n; ++i)
	  {
		sd_write_chunk[i] = ring_buf[ring_tail];
		ring_tail = (ring_tail + 1U) & (RING_BUF_SAMPLES - 1U);
	  }

	  FRESULT fres = f_write(&SDFile, sd_write_chunk, n * sizeof(int32_t), &bw);
	  if ((fres != FR_OK) || (bw != (n * sizeof(int32_t))))
	  {
		char err[96];
		int len = snprintf(err, sizeof(err),
						   "SD ERR: fres=%d, bw=%u/%u, written=%lu, addr=0x%08lX\r\n",
						   (int)fres, (unsigned int)bw,
						   (unsigned int)(n * sizeof(int32_t)),
						   (unsigned long)samples_written,
						   (unsigned long)(uintptr_t)sd_write_chunk);
		TransmitUartText(err, (uint16_t)len);
		Error_Handler();
	  }
	  samples_written += n;
	}
}

  (void)HAL_I2S_DMAStop(&hi2s1);
  if (f_sync(&SDFile) != FR_OK) Error_Handler();
  if (f_close(&SDFile) != FR_OK) Error_Handler();

  char msg_peak[96];
  (void)snprintf(msg_peak, sizeof(msg_peak),
                 "=== Capture peak: %ld (24-bit) ===\r\n",
                 (long)capture_peak);
  TransmitUartText(msg_peak, (uint16_t)strlen(msg_peak));

  const char msg_conv[] = "=== Converting BIN -> WAV & CSV (please wait ~5s) ===\r\n";
  TransmitUartText(msg_conv, (uint16_t)(sizeof(msg_conv) - 1U));

  CreateWavAndCsvFromBin(bin_path, wav_path, csv_path);
  (void)f_mount(NULL, (TCHAR const *)SDPath, 0U);

  const char msg_done[] = "=== Done! Check .bin, .wav, .csv on SD ===\r\n";
  TransmitUartText(msg_done, (uint16_t)(sizeof(msg_done) - 1U));
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	const char msg_conv[] = "=== ERR ===\r\n";
	  TransmitUartText(msg_conv, (uint16_t)(sizeof(msg_conv) - 1U));
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

# Thu tín hiệu I2S 32 kHz trong 10 giây

Tài liệu này mô tả chương trình trong `phy-prjecobj-h723-pacup` dùng STM32H723 để:

- tạo clock I2S1 ở 32 kHz;
- nhận mẫu I2S bằng DMA circular;
- thu đúng 320.000 mẫu trong 10 giây;
- ghi mẫu thành file `i2s_32k_10s.csv` trên thẻ SD qua FATFS/SDMMC1;
- đồng thời gửi mẫu qua LPUART1.

## 1. Kết nối phần cứng

Cấu hình hiện tại dùng các chân sau:

| Tín hiệu | STM32H723 |
| --- | --- |
| I2S WS | PA4 |
| I2S CK | PA5 |
| I2S SDI | PA6 |
| I2S SDO | PA7 |
| LPUART1 TX | PA9 |
| LPUART1 RX | PA10 |
| SDMMC1 D0..D3 | PC8..PC11 |
| SDMMC1 CK | PC12 |
| SDMMC1 CMD | PD2 |

I2S1 đang ở `I2S_MODE_MASTER_FULLDUPLEX`. Vì vậy PA5/PA4 được tạo bởi STM32; thiết bị phát tín hiệu cần nối dữ liệu vào PA6 và dùng WS/CK từ STM32. PA7 là đường phát dữ liệu dummy để duy trì clock trong full-duplex.

Thẻ SD phải được format FAT/FAT32 và lắp trước khi reset hoặc chạy chương trình.

## 2. Luồng dữ liệu

```text
I2S1 SDI -> DMA1 Stream1 -> buffer RX 2048 mẫu uint32_t
                         -> callback half/full
                         -> main loop
                            |-> CSV qua FatFs -> SDMMC1 -> thẻ SD
                            `-> 4 byte/mẫu -> LPUART1 TX
```

DMA dùng hai nửa buffer, mỗi nửa 1024 mẫu, tương đương 32 ms dữ liệu ở 32 kHz. Buffer TX dummy và RX được đặt trong section `.dma_buffer` ở RAM_D2 để DMA1 truy cập được.

## 3. Định dạng file CSV

File được tạo tại volume FATFS của SD:

```text
0:/i2s_32k_10s.csv
```

Nội dung có dạng:

```text
sample_index,sample_24bit
0,1234
1,-5678
```

I2S đang cấu hình `I2S_DATAFORMAT_24B`. Giá trị nhận được được lấy 24 bit và sign-extend thành `int32_t` trước khi ghi CSV. Chỉ số mẫu chạy từ `0` đến `319999`.

## 4. Định dạng dữ liệu trên LPUART1

Để truyền liên tục mà không làm mất mẫu, LPUART1 được đặt ở 3.000.000 baud, 8-N-1. UART không truyền chuỗi CSV; UART truyền từng mẫu dưới dạng một word 32-bit little-endian, trong đó 24 bit thấp là dữ liệu I2S. Phía máy tính có thể đọc 4 byte mỗi mẫu và sign-extend 24 bit tương tự như mã firmware.

Tốc độ cần thiết là:

```text
32.000 mẫu/s * 4 byte/mẫu = 128.000 byte/s
```

Tốc độ 3 Mbps đủ cho luồng này. Truyền CSV text ở baud 209.700 hiện tại không đảm bảo đủ băng thông cho các mẫu âm thanh 24-bit.

## 5. Các phần mã đã bổ sung

### `Core/Src/main.c`

- khai báo buffer DMA RX/TX;
- cài callback `HAL_I2SEx_TxRxHalfCpltCallback` và `HAL_I2SEx_TxRxCpltCallback`;
- mount SD bằng `f_mount`;
- mở file bằng `f_open`;
- gọi `HAL_I2SEx_TransmitReceive_DMA`;
- xử lý từng nửa buffer, gửi UART và gọi `f_write`;
- dừng DMA sau 320.000 mẫu, `f_sync`, `f_close`, rồi unmount.

### `Core/Src/stm32h7xx_hal_msp.c`

DMA I2S được đặt:

- TX: `DMA1_Stream0`, word-to-word, circular;
- RX: `DMA1_Stream1`, word-to-word, circular.

### `STM32H723ZGTX_FLASH.ld`

Thêm section `.dma_buffer` vào RAM_D2.

### `phy-prjecobj-h723-pacup.ioc`

Đồng bộ lại DataAlignment word và TX circular để lần regenerate tiếp theo của CubeMX không ghi đè cấu hình cần thiết.

## 6. Build bằng STM32CubeIDE

1. Mở thư mục `phy-prjecobj-h723-pacup` trong STM32CubeIDE.
2. Chọn project và cấu hình `Debug` hoặc `Release`.
3. Chọn **Project > Clean** nếu project trước đó chưa có FATFS.
4. Chọn **Project > Build Project**.
5. Flash chương trình vào STM32H723.
6. Mở terminal UART ở 3.000.000 baud, 8-N-1 nếu cần nhận luồng binary.
7. Reset board và chờ khoảng 10 giây.
8. Tháo thẻ SD an toàn sau khi chương trình đã quay lại vòng lặp chính, sau đó kiểm tra file CSV.

## 7. Kiểm tra kết quả

Trên máy tính, file cần có 320.001 dòng: một dòng header và 320.000 dòng dữ liệu. Có thể kiểm tra bằng:

```bash
wc -l i2s_32k_10s.csv
```

Giá trị kỳ vọng là:

```text
320001 i2s_32k_10s.csv
```

## 8. Lưu ý khi tích hợp CubeMX

- Không đổi `I2S_DATAFORMAT_24B` sang 16-bit nếu vẫn dùng bộ giải mã 24-bit trong `DecodeI2S24`.
- Nếu thay đổi baud UART, phải bảo đảm tốc độ truyền tối thiểu khoảng 1,28 Mbps với dữ liệu 4 byte/mẫu và 8-N-1.
- `BSP_SD_IsDetected()` hiện được driver sinh ra trả về `SD_PRESENT`. Nếu board có chân detect riêng, cần thay weak implementation trong `FATFS/Target/bsp_driver_sd.c`.
- Code hiện ghi CSV trong main loop. Nếu thẻ SD có thời gian ghi bất thường dài hơn 32 ms, cần tăng kích thước buffer hoặc chuyển sang kiến trúc ring buffer nhiều block để tránh DMA ghi đè dữ liệu chưa xử lý.
- Chương trình tự động ghi đè file cũ bằng `FA_CREATE_ALWAYS`.

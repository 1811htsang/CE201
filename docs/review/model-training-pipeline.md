# Pipeline huấn luyện AI

> Tài liệu này mô tả pipeline huấn luyện AI offline trên PC

## 1. Phạm vi

Theo Mục 5 M0, pipeline train tách biệt hoàn toàn khỏi firmware: chạy bằng Python/TensorFlow trên PC, đầu ra là 1 file `.tflite` int8 + file test set kết quả để báo cáo. Không có dòng Python nào chạy trên MCU.

```text
Dữ liệu thô (MIMII/ToyADMOS/DCASE + tự thu)
      │
      ▼
manifest.csv  ──────────────────────────────┐ (schema thống nhất, bất kể nguồn)
      │                                      │
      ▼                                      │
group-split train/val/test (theo file,       │
tránh rò rỉ khung cùng 1 file)               │
      │                                      │
      ▼                                      │
Trích MFCC (float32, librosa) ◄──────────────┘
      │
      ▼
Train MLP/CNN1D float32  ──► đánh giá trên tập test (accuracy/precision/recall/F1)
      │
      ▼
PTQ int8 (TFLite Converter, representative dataset từ tập train)
      │
      ▼
model_int8.tflite  ──► đánh giá lại trên tập test, so sánh với float32
      │
      └──► model_int8.h (C header, nhúng thẳng vào firmware nếu cần)
```

## 2. Một artifact, hai board — điểm làm rõ so với M0

Mục 3.2 của M0 viết: *"nhánh TFLite Micro (ESP32-S3) và CMSIS-NN (STM32H723)"*, dễ đọc thành 2 quy trình export khác nhau. Thực tế:

- **TFLite Micro** là interpreter (vòng lặp đọc graph `.tflite` rồi gọi kernel).
- **CMSIS-NN** và **esp-nn** là 2 thư viện kernel toán học (Conv/FC int8 tối ưu theo tập lệnh riêng của Cortex-M và Xtensa) mà TFLite Micro gọi xuống khi cần tính.

Nói cách khác: cùng một file `model_int8.tflite` chạy được trên cả 2 board, khác nhau chỉ ở việc build firmware link với thư viện kernel nào (`CMSIS-NN` cho STM32H723, `esp-nn` cho ESP32-S3). Pipeline train vì vậy chỉ cần xuất một artifact duy nhất (`src/quantize.py`), không cần 2 nhánh export riêng.

## 3. Các quyết định thiết kế & lý do

| Quyết định | Lý do |
| --- | --- |
| `center=False` khi trích MFCC (không pad/reflect quanh mép khung) | Firmware thu I2S theo luồng liên tục, không ai làm reflect-padding trên MCU. Nếu để `center=True` (mặc định của librosa), 2 bên sẽ lệch khung ngay từ đầu, chưa cần tới sai số lượng tử hoá cũng đã trượt — phá luôn phép đo "Sai lệch MFCC" ở Mục 6 M0. |
| Cắt audio thành cửa sổ không chồng lấp (non-overlapping) trước khi trích MFCC | TASK_INFER trên firmware chạy mỗi khung 1 lần, không suy luận lặp lại trên dữ liệu trùng nhau. Train bằng sliding window chồng lấp sẽ cho số lượng mẫu ảo nhiều hơn thực tế chạy trên board, làm sai lệch cả số liệu accuracy lẫn ước lượng benchmark. |
| Chia train/val/test theo từng file (không theo machine_id) | Gộp theo machine_id có thể làm rỗng tập val/test khi 1 loại máy chỉ có vài machine_id. |
| GlobalAveragePooling1D thay vì Flatten trước Dense cuối (CNN1D) | Số tham số không phụ thuộc `frames_per_window`, đổi `window_duration_s` trong config không phải thiết kế lại kiến trúc. |
| Không dùng BatchNorm ở bản đầu | Fold BatchNorm vào Conv lúc convert sang int8 đôi khi cho scale/zero-point không tối ưu, khó debug khi mới bắt đầu. Có thể thêm lại sau nếu cần. |
| PTQ trước, chỉ cân nhắc QAT nếu PTQ tụt quá ngưỡng | Đúng theo Mục 4.2 M0: *"Không dùng QAT trừ khi PTQ làm độ chính xác giảm quá ngưỡng chấp nhận"*. |
| Representative dataset cho PTQ lấy từ tập train thật (không phải random) | TFLite Converter cần biết phân bố giá trị thật của MFCC để chọn range int8 hợp lý; dùng số ngẫu nhiên sẽ cho range sai, kéo accuracy int8 xuống thấp. |

## 4. Các tham số cụ thể (đề xuất ban đầu)

| Tham số | Giá trị đề xuất | Vì sao |
| --- | --- | --- |
| `sample_rate_hz` | 16000 | Theo M0 ("dự kiến 16kHz trở lên") |
| `n_fft` / `win_length` | 512 (= 32ms @16kHz) | Cỡ FFT 2 lũy thừa, khớp hàm FFT có sẵn của CMSIS-DSP (`arm_rfft_fast_f32`/q15) |
| `hop_length` | 256 (= 16ms, 50% overlap giữa khung FFT) | Tỉ lệ hop/win phổ biến cho MFCC giọng nói/âm thanh công nghiệp |
| `n_mfcc` | 13 | Đủ phân biệt phổ âm thanh cơ khí, giữ vector đặc trưng nhỏ cho MCU |
| `frames_per_window` | 61 (≈ 1 giây ngữ cảnh) | Cân bằng giữa đủ ngữ cảnh thời gian và kích thước input model |
| `min_f1` | 0.90 | Lấy thẳng từ Mục 6 M0 |
| `max_mfcc_relative_error` | 0.05 (5%) | Đề xuất |
| `max_accuracy_drop_int8` | 0.03 (3%) | Đề xuất |

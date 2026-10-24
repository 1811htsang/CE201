# To-do: Triển khai Pipeline huấn luyện AI

---

## Phase 0 — Chuẩn bị môi trường & dữ liệu thô

- [ ] Tạo virtualenv/conda env riêng, cài `tensorflow`, `librosa`, `numpy`, `pandas`, `scipy`
- [ ] Tải **UOEMD-VAFCVS** từ Mendeley (DOI 10.17632/msxs4vj48g)
- [ ] Tải **UORED-VAFCLS** từ Mendeley
- [ ] Tải **GUET Multi-Condition Acoustic Ball Bearing** từ Mendeley
- [ ] Tìm/cài tool đọc file `.bkc` của GUET (SDK B&K Connect, hoặc tool convert cộng đồng) — xác nhận đọc được trước khi code pipeline
- [ ] Review nhanh 1 file mẫu mỗi bộ bằng tay (mở `.mat`/`.csv` bằng `scipy.io.loadmat`/`pandas`, nghe thử `.bkc` sau khi convert) — xác nhận đúng số kênh, đúng tần số ghi trong paper

## Phase 1 — Chuẩn hoá dữ liệu thô → `manifest.csv`

- [ ] Viết script đọc `.mat`/`.csv` của UOEMD-VAFCVS/UORED-VAFCLS, trích đúng cột âm thanh (microphone), bỏ các cột accelerometer/tải/tốc độ (hoặc giữ lại làm metadata riêng)
- [ ] Viết script convert `.bkc` (GUET) → `.wav`/numpy array
- [ ] Viết script resample chung về 32kHz cho cả 3 bộ (42k→32k, 32.768k→32k) — dùng bộ lọc anti-alias (`scipy.signal.resample_poly` hoặc `librosa.resample`), không decimate thô
- [ ] Với GUET: cắt file liên tục 10 phút thành các đoạn ngắn hơn (khớp độ dài cửa sổ dùng ở Phase 3), gán nhãn theo đúng mốc thời gian chuyển trạng thái lỗi ghi trong paper
- [ ] Thiết kế schema `manifest.csv`: tối thiểu `file_path, label (normal/abnormal), source_dataset, fault_type, rpm, load, duration_s`
- [ ] Viết script quét toàn bộ dữ liệu đã chuẩn hoá, sinh `manifest.csv`
- [ ] Sanity check: đếm số file/giây audio theo từng `label` và từng `source_dataset` — phát hiện sớm nếu lệch nhãn hoặc tập quá mất cân bằng

## Phase 2 — Chia train/val/test

- [ ] Viết hàm group-split theo file (không theo `machine_id`, đúng quyết định đã chốt) — dùng `sklearn.model_selection.GroupShuffleSplit` hoặc tự viết
- [ ] Set tỉ lệ split (vd 70/15/15), cố định `random_seed` để tái lập được
- [ ] Kiểm tra lại: mỗi tập train/val/test có đủ cả normal lẫn abnormal, đủ đại diện cả 3 `source_dataset` (không để val/test chỉ toàn 1 bộ)

## Phase 3 — Trích đặc trưng MFCC

- [ ] Viết hàm cắt audio thành cửa sổ không chồng lấp (non-overlapping) trước khi trích MFCC
- [ ] Viết hàm trích MFCC bằng `librosa`, đúng tham số đã chốt: `sample_rate=32000, n_fft=1024, hop_length=512, n_mfcc=13, center=False`
- [ ] Ghép `frames_per_window=311` khung hop/segment (≈5s)
- [ ] Cache kết quả MFCC ra `.npy`/`.npz` theo từng file trong manifest, tránh tính lại mỗi lần train (tiết kiệm thời gian lặp thử nghiệm)
- [ ] Viết 1 script visualize nhanh vài sample MFCC (normal vs abnormal) để mắt thường kiểm tra có phân biệt được không, trước khi tốn công train model

## Phase 4 — Train model float32

- [ ] Code kiến trúc MLP hoặc CNN1D, dùng GlobalAveragePooling1D trước Dense cuối (không Flatten), không BatchNorm ở bản đầu
- [ ] Viết training loop: loss (binary/categorical crossentropy), optimizer, early stopping theo val loss
- [ ] Train, log lại accuracy/precision/recall/F1 trên tập test
- [ ] So với ngưỡng `min_f1 = 0.90` (Mục 6 M0) — nếu chưa đạt: thử tăng `n_mfcc`, đổi kiến trúc, hoặc quay lại Phase 3 thử frames_per_window khác
- [ ] Lưu model float32 (`.h5`/SavedModel) làm baseline so sánh với bản int8 sau

## Phase 5 — Lượng tử hoá PTQ (int8)

- [ ] Viết `src/quantize.py`: dùng TFLite Converter, representative dataset lấy từ tập train thật
- [ ] Export `model_int8.tflite`
- [ ] Đánh giá lại accuracy/F1 trên tập test với model int8, so với float32
- [ ] Check `max_accuracy_drop_int8 = 0.03` (3%) — nếu vượt ngưỡng: cân nhắc QAT (chỉ khi PTQ thật sự không đạt, đúng nguyên tắc Mục 4.2 M0)
- [ ] Đo `max_mfcc_relative_error = 0.05` (5%) — so MFCC tính trên PC (float32, `center=False`) với MFCC sẽ tính trên firmware (CMSIS-DSP, cố định điểm hoặc float32 tuỳ cấu hình)

## Phase 6 — Export cho firmware

- [ ] Xác nhận lại với leader: giữ nguyên đường TFLite Micro (nạp `.tflite` + interpreter, CMSIS-NN chỉ là kernel)
- [ ] Export `model_int8.h` (C header) nếu cần nhúng thẳng trọng số vào firmware
- [ ] Thống nhất với phía firmware (μEDP) format message/vector đặc trưng giữa TASK_FEAT → TASK_INFER

## Phase 7 — Báo cáo & đồng bộ tài liệu

- [ ] Viết bảng so sánh float32 vs int8 (accuracy, F1, kích thước model, thời gian suy luận ước tính) cho báo cáo M1
- [ ] Nếu phát sinh thay đổi tham số/kiến trúc so với `pipeline-huan-luyen-ai.md` trong lúc code thực tế — quay lại cập nhật file đó, giữ tài liệu luôn khớp với code thật
- [ ] Review lại toàn bộ `manifest.csv` cuối cùng — lưu kèm báo cáo để tái lập được thí nghiệm

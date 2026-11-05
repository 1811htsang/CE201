# To-do list

List này thực hiện trình bày toàn bộ danh sách các công việc dành cho đồ án CE201.

## Attendees

- Nguyễn Duy Xuân Bách
- Huỳnh Thanh Sang
- Nguyễn Hoàng Hải Minh

## Linked Document

- [review_planning_ce201](review_planning_ce201.pdf)
- [μEDP](https://github.com/1811htsang/uEDP)
- [Predictive Maintenance via Acoustic Fingerprint](https://github.com/1811htsang/Predictive-Maintenance-via-Acoustic-Fingerprint-on-ESP32-S3)

## To-do Items

- [x] Chốt hướng đi theo đề xuất của giảng viên. //NOTE - Hướng A với đề xuất là μEDP + Predictive Maintenance via Acoustic Fingerprint.
- [x] Rewrite README.md để trình bày tổng quan đồ án mà môn học CE201 đang thực hiện. //NOTE - Chỗ này sẽ để Minh thực hiện kết hợp file review + README μEDP.
- [x] Review lại tài liệu liên quan đến repo Predictive Maintenance sẵn có. //NOTE - Thực hiện việc kiểm tra lại các tính năng, tài liệu, thiết kế hardware.
- [x] Viết đề cương theo khung Mục 7 (đặc biệt Chương 1 & 4) để chuẩn bị cho việc review với GV.
- [x] Add thêm thông tin về các bộ dataset sử dụng trong đề tài.

<!-- REVIEW
https://github.com/VictorBauler/awesome-bearing-dataset

Xem trong repository này để select các audio dataset cần thiết cho việc thực hiện thí nghiệm Predictive Maintenance. Cần thống nhất với GV về các bộ dataset sử dụng trong đồ án.
-->

- [x] Sửa đổi bổ sung lại schematic và PCB design của repo PdM cũ.
- [x] Revert lại các porting MCU trước đó, tập trung vào STM32H723, ESP32S3.

<!-- STATUS
Hiện tại đã revert lại các porting MCU trước đó, tập trung vào STM32H723, ESP32S3 với việc bổ sung API implementation thật sự theo yêu cầu task BST từ phía repo μEDP. Ngoại lệ STM32F103 được giữ lại để thực hiện thí nghiệm baseline. Các MCU khác sẽ được loại bỏ khỏi repo CE201.
-->

- [x] Review lại các tính năng cần được escalate development để hỗ trợ cho CE201.

<!-- STATUS
Hiện tại thì đã cân nhắc đưa PLTF và AMP/HELF vào trong lộ trình phát triển hỗ trợ cho CE201 nhưng cần cân nhắc lại timeline.

Đã đưa vào lộ trình sau PLTF.
-->

- [x] Bổ sung API cho uutobj F103 để làm tham khảo cho uutobj H723.
- [x] @! TID DOCS-001 <+ Cập nhật nội dung các bài báo nghiên cứu cần thực hiện literature review trong tài liệu.

<!-- STATUS
Hiện tại thì μEDP đã chính thức ra mắt releáe v1.1.6 nên có thể cân nhắc chính thức đưa vào sử dụng trong đồ án CE201. Bắt đầu chuyển qua giai đoạn thực hiện thí nghiệm với μEDP v1.1.6.
-->

- [x] @! TID DOCS-002 <+ Bổ sung lại thông tin các bài báo nghiên cứu cần thực hiện literature review trong tài liệu.

<!-- STATUS
Hiện tại đã bổ sung ở `docs/references`
Tuy nhiên ở repo PdM-AF đã có sẵn các bài báo nghiên cứu cần thực hiện literature review trong tài liệu nên mở rộng hướng nghiên cứu để đạt một số lượng bài báo nghiên cứu cần thực hiện literature review trong tài liệu.

# DEPRECATED - Old TASK - Phân công nhiệm vụ cho các thành viên trong nhóm để thực hiện literature review và tổng hợp lại thành một tài liệu chung.
-->

- [x] @! TID[epic=REPO,seq=62] REPO-001 <+ Bổ sung lại thông tin các bài báo nghiên cứu cần thực hiện literature review trong tài liệu.
- [x] @! TID[epic=PCB] PCB-001 <+ Bổ sung sử dụng LTSpice để mô phỏng mạch điện tử trong schematic và PCB design của repo PdM cũ.

<!-- STATUS
Đã bổ sung sử dụng LTSpice để mô tả và thiết kế mạch khuếch đại tín hiệu thay thế cho thiết kế schematics và PCB design của repo PdM cũ. 

Cần cân nhắc bổ sung review của thầy Nguyên để đảm bảo tính đúng đắn của thiết kế.
-->

- [x] @! TID[epic=ALGO] INF-001 <+ Bổ sung lộ trình thiết kế API chi tiết cho thuật toán sẽ sử dụng. -> Minh đang làm.
- [x] @! TID[epic=ALGO] INF-002 <+ Bổ sung tài liệu tracking làm việc cho pipeline training model.
- [ ] @! DEPRECATED PCB-002 <+ Cập nhật và sửa đổi PCB design với TCT40-16R thay bằng INMP441.
- [x] @! TID[epic=ALGO] PCB-002 <+ Hoàn thiện việc mô tả và thiết kế mạch khuếch đại tín hiệu thay thế cho thiết kế schematics và PCB design của repo PdM cũ bằng LTSpice.

<!-- STATUS
Đã có schematic chi tiết có thể triển khai cập nhật vào PCB design của repo PdM cũ. Cần cân nhắc bổ sung review của thầy Nguyên để đảm bảo tính đúng đắn của thiết kế.
-->

- [x] @! TID[epic=MCU] MCU-001 <+ Bổ sung hoàn chỉnh các API cần thiết cho lớp PAL trên hardware chủ chốt STM32H723 và ESP32S3

<!-- SECTION

//LINK sources/uEDP/docs/to-do.md:554

```markdown
- [x] Thực hiện bổ sung C-type API cho archobj STM32F103 (abbr F103), archobj STM32H723 (abbr H723) và archobj ESP32S3 (abbr S3N16) để hỗ trợ việc kiểm tra khả năng sinh code và thực thi các cấu hình logic của μE-LS từ các mô tả logic trong PLD.
```

#STATUS 

Trong quá trình thực hiện bổ sung API cần thiết cho uutobj F103, đã phát hiện ra việc triển khai API sẽ chỉ cần tập trung vào các phương diện cần thiết thay vì triển khai toàn bộ.
Do đó, BSP của các MCU chủ chốt chỉ cần được khai báo toàn bộ vào từng PAL nhưng việc triển khai sẽ phụ thuộc vào người dùng và các tính năng cần thiết cho CE201.

#!SECTION
-->

- [x] @! TID[epic=DOCS] DOCS-003 <+ Thống nhất về bộ công cụ sử dụng cho toàn bộ đồ án.

<!-- REVIEW
Version control: GitHub/Git
IDE: VSCode, STM32CubeIDE/CubeMX, ESP-IDF
Content: VSCode, Comment Anchor, Google Docs, Notion
-->

- [x] Sắp xếp meeting với GV để thống nhất hướng đi và timeline.
- [x] @! TID[epic=REPO,seq=107] REPO-002 <+ Thống nhất về timeline và release selection của μEDP nhằm đảm bảo tính ổn định.

<!-- STATUS
Timeline hiện tại đã thống nhất theo mốc M0-M5 của đồ án 1.
μEDP đã chốt ở release v1.1.6 để đảm bảo tính ổn định cho việc thực hiện thí nghiệm trong đồ án. Tuy nhiên nếu tồn tại các bản bugfix thì vẫn có thể chuyển sang nhưng không chuyển sang phiên bản mới hơn v1.1.6.
-->

- [x] Thống nhất về các testobj và testbench cần thiết để thực hiện các thí nghiệm trong đồ án.

<!-- STATUS
Đã có thống nhất nhưng chưa mua.
-->

- [x] Review lại phân công công việc trong Mục 6.4 để thống nhất với GV.
- [ ] Bổ sung file báo cáo riêng biệt để làm tổng hợp nghiên cứu và thực hiện literature review cho báo cáo đồ án CE201.
- [x] @! TID[epic=PCB,seq=64] PCB-003 <+ Chuyển đổi schematic sang PCB design để thực hiện việc in ấn PCB cho thí nghiệm CE201.
- [x] @! TID[epic=PCB,seq=65] PCB-005 <+ Cập nhật chân 1x5 header lên 1x6 theo mạch PCM1808.
- [x] @! TID[epic=ORDER,seq=127] PCB-006 <+ Bổ sung linh kiện motor và các thiết kế cho testbench.
- [ ] @! TID[epic=ORDER,seq=128] PCB-007 <+ Cập nhật bổ sung thanh trượt 10mm/8mm với ốc M2 dài 6mm để khóa trục.
- [x] @! TID[epic=ORDER,seq=129] PCB-008 <+ Thực hiện đặt gia công mạch PCB cho thí nghiệm CE201.
- [x] @! TID[epic=PCB,seq=66] PCB-004 <+ Bổ sung mạch trung gian 2x20 để khớp với chân mạch H723 và hỗ trợ mapping chân header 1x6 sang 2x20.
- [x] @! TID[epic=MCU,seq=81] MCU-002 <+ Bổ sung các I2S API cho uutobj H723.
- [ ] @! TID[epic=MCU,seq=82] MCU-003 <+ Thiết kế lstaxizer.yaml cho đồ án.

<!-- SECTION - Firmware configuration for AI pipeline
# NOTE - Current status

Hiện tại dự án chưa có cấu hình `.config` cụ thể cho pipeline AI. Bảng dưới đây chỉ là cấu hình mục tiêu để nhóm thống nhất và tạo mới sau khi GVHD chốt; không được xem `phy-prjecobj-h723/uEDP/deprecated/.config` là cấu hình đang dùng.
Model int8 chưa tồn tại, vì vậy các tham số phụ thuộc model vẫn để chờ. Không sử dụng PSE trong pipeline đầu tiên.

# NOTE - Hạng mục và giá trị mục tiêu trong bảng dưới đây chỉ là đề xuất ban đầu để thảo luận; không phải cấu hình hiện hữu và không phải kết quả đo. Sau khi có harness, phải đo frame drop, message allocation failure và backlog trước khi tăng/giảm chúng.

| Hạng mục | Giá trị mục tiêu | Ghi chú |
| --- | --- | --- |
| PAL | `stm32h723` | Tận dụng API I2S DMA hiện có: bind handle, đăng ký callback, start/stop RX. |
| Task norm | **Đề xuất ban đầu: `4`**: `TASK_ACQ`, `TASK_FEAT`, `TASK_INFER`, `TASK_DISP` | Số lượng chưa chốt; phải kiểm tra lại theo logic DMA/frame buffer, MFCC, inference và output cảnh báo. Có thể gộp/tách task nếu benchmark hoặc ràng buộc run-to-completion yêu cầu. |
| Task poll | `0` | Không dùng polling task trong baseline AI. |
| FSM/TSM | `0` cho cả 4 task | Dùng handler tuyến tính trước; chỉ thêm HSMC/TSM nếu đo được inference vượt ngân sách thời gian. |
| Signal ứng dụng | **Đề xuất ban đầu: `4`**: `SIG_ACQ_FRAME_READY`, `SIG_FEAT_DONE`, `SIG_INFER_DONE`, `SIG_ACQ_ERROR` | Số lượng chưa chốt; signal phải được suy ra từ các đường điều khiển thực tế, không tạo signal chỉ để đủ số lượng. |
| Hardware API | `4` API I2S | Chỉ khai báo route cần cho pipeline; không khai báo trùng các API HAL/CubeMX đã có trong BSP. |
| PSE | Không dùng | Không có `psetbl`, `uedp_msg_register_topic()` hoặc `uedp_msg_publish()`; các task gửi message trực tiếp tới task kế tiếp. |

# NOTE - Logic cần dùng để chốt task và signal

Bốn task và bốn signal ở trên chỉ là baseline để thảo luận. Cấu hình cuối cùng phải được lập từ bảng logic sau và có thể thay đổi trước khi tạo `.config` mới:

| Đường logic | Bên phát | Bên nhận | Signal đề xuất | Message tối thiểu | Điều kiện cần xác minh |
| --- | --- | --- | --- | --- | --- |
| DMA hoàn tất một frame | ISR/callback I2S → bridge | `TASK_ACQ` | `SIG_ACQ_FRAME_READY` | chỉ thị buffer/frame index, không truyền cả buffer qua queue | callback chỉ dùng API ISR-safe; buffer không bị ghi đè trước khi `TASK_FEAT` dùng xong |
| Frame đã sẵn sàng để trích đặc trưng | `TASK_ACQ` | `TASK_FEAT` | có thể dùng lại `SIG_ACQ_FRAME_READY` hoặc signal riêng | con trỏ/index buffer và độ dài frame | quyết định rõ ACQ chỉ quản lý buffer hay FEAT nhận quyền sở hữu buffer |
| MFCC hoàn tất | `TASK_FEAT` | `TASK_INFER` | `SIG_FEAT_DONE` | vector MFCC hoặc vùng nhớ cố định + metadata | kích thước vector, Q-format và ownership phải khớp pipeline training |
| Inference hoàn tất | `TASK_INFER` | `TASK_DISP` | `SIG_INFER_DONE` | lớp dự đoán, score/quantized output và timestamp | output tensor/model API chưa thể chốt khi chưa có model int8 |
| DMA/I2S lỗi | ISR/callback I2S → bridge | `TASK_ACQ` hoặc service lỗi | `SIG_ACQ_ERROR` | mã lỗi và timestamp | cần xác nhận có cần task/service lỗi riêng hay chỉ log và khởi động lại ACQ |

Theo bảng này, một frame thành công có thể cần **3 hoặc 4 signal điều khiển** tùy việc `TASK_ACQ` có cần signal nội bộ riêng hay message từ callback được ánh xạ trực tiếp sang `TASK_FEAT`. `SIG_ACQ_ERROR` là nhánh lỗi, không phải đường dữ liệu thành công. Vì vậy chưa được kết luận rằng cấu hình cuối cùng luôn là 4 task norm và 4 signal.

# NOTE - Kích thước tài nguyên khởi đầu để benchmark

Các số dưới đây là giá trị khởi đầu cần chốt, không phải cấu hình hiện hữu và
không phải kết quả đo. Sau khi có harness, phải đo frame drop, message allocation
failure và backlog trước khi tăng/giảm chúng.

| Kconfig | Giá trị đề xuất | Mục đích |
| --- | ---: | --- |
| `CORE_UEDP_MSG_BLANK_QUEUE_SIZE` | `8` | Hàng đợi riêng cho mỗi task norm. |
| `CORE_UEDP_MSG_ISR_QUEUE_SIZE` | `8` | Sự kiện DMA/error được đẩy ra ngoài ISR. |
| `CORE_UEDP_MSG_ALLOC_QUEUE_SIZE` | `16` | Dự phòng message đang chờ trong pipeline 4 bước. |
| `CORE_UEDP_MSG_ALLOC_N_VALUE` | `4` | Giới hạn cấp phát tĩnh ban đầu. |
| `CORE_UEDP_MSG_EXTAL_QUEUE_SIZE` | `8` | Luồng debug/ngoài hệ thống, không dùng cho dữ liệu AI. |
| `CORE_UEDP_TASK_MSG_QUEUE_SIZE` | `8` | Queue message của task khi không dùng PSE. |
| `CORE_UEDP_TIMER_MAX_NODES` | `8` | Giữ mức cơ sở cho runtime; chưa có yêu cầu timer riêng từ model. |
| `CORE_UEDP_ITNLOG_MAX_LOG_ENTRIES` | `32` | Đủ cho harness ban đầu. |
| `CORE_UEDP_ITNLOG_FLUSH_THRESHOLD` | `28` | Giá trị khởi đầu đi cùng log capacity ở trên. |

# NOTE - Các tham số model-dependent chưa thể chốt

- Chốt một cấu hình duy nhất cho sample rate, số mẫu/frame, độ dài cửa sổ, số hệ số MFCC và Q-format. M0/M1 có nêu 32 kHz nhưng tài liệu pipeline còn có cấu hình 16 kHz/5 s; phải giải quyết mâu thuẫn trước khi định kích thước buffer firmware.
- Chưa có `model_int8.tflite`, nên chưa thể chốt input/output tensor shape, scale/zero-point, `MicroMutableOpResolver`, danh sách operator hoặc tensor arena.
- CMSIS-NN hiện là kernel library; nó không tự parse `.tflite`. Cần chốt TFLite Micro hoặc wrapper trọng số trực tiếp sau khi artifact model có thật.
- `lib-cmsisnn/CMakeLists.txt` đã có target `cmsis-nn` cho Cortex-M7, nhưng còn phải nối target này vào executable H723 và xác nhận thống nhất `-mcpu`, `-mfpu`, `-mfloat-abi` và mức tối ưu với firmware.

#!SECTION
-->

### Việc cần cập nhật theo tiến độ

- [ ] @! TID[epic=ALGO,seq=136] ALGO-003 <+ Chốt sample rate/frame/MFCC/Q-format giữa M0, M1 và pipeline training.
- [ ] @! TID[epic=MCU,seq=137] MCU-004 <+ Phân tích logic DMA/frame buffer và message route để chốt số task norm, signal, message payload và ownership; 4 task/4 signal hiện chỉ là đề xuất, không sửa `deprecated/.config`.
- [ ] @! TID[epic=MCU,seq=138] MCU-005 <+ Thiết kế `lstaxizer.yaml` không dùng PSE, mô tả route trực tiếp ACQ → FEAT → INFER → DISP và kiểm tra output sinh code.
- [ ] @! TID[epic=ALGO,seq=139] ALGO-004 <+ Huấn luyện/evaluate model float32, chuyển PTQ int8, ghi quantization parameters và tạo artifact `.tflite`.
- [ ] @! TID[epic=MCU,seq=140] MCU-006 <+ Nối target `cmsis-nn` vào build H723 và xác nhận link bằng `arm-none-eabi-gcc`.
- [ ] @! TID[epic=ALGO,seq=141] ALGO-005 <+ Đo arena, latency từng task, p99/jitter, frame drop và pool exhaustion trên input replay.

<!-- STATUS
Chỉ đánh dấu các mục trên hoàn tất khi có `.config` và `lstaxizer.yaml` mới trong nhánh H723, PLTF sinh được declaration/app, CMSIS-NN link được vào firmware và các tham số model-dependent được lấy từ artifact thật.
-->

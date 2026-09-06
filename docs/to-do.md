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
- [ ] Add thêm thông tin về các bộ dataset sử dụng trong đề tài.
- [ ] Bổ sung lại thông tin các bài báo nghiên cứu cần thực hiện literature review trong tài liệu.
- [ ] Revert lại các porting MCU trước đó, tập trung vào STM32H723, ESP32S3. // NOTE - Phần này sẽ để Sang thực hiện
- [ ] Bổ sung hoàn chỉnh các API cần thiết cho lớp PAL trên hardware chủ chốt STM32H723 và ESP32S3. //REVIEW - Minh sẽ thực hiện phần này, có thể sử dụng AI để propose các API cần thiết.
- [ ] Review lại các tính năng cần được escalate development để hỗ trợ cho CE201.

<!-- STATUS
Hiện tại thì đã cân nhắc đưa PLTF và AMP/HELF vào trong lộ trình phát triển hỗ trợ cho CE201 nhưng cần cân nhắc lại timeline.
-->

- [ ] Thống nhất về bộ công cụ sử dụng cho toàn bộ đồ án. //NOTE - chỗ này sẽ bao gồm các công cụ về content, code, version control, CI/CD, testing, v.v. để thống nhất với GV.

<!-- REVIEW
Version control: GitHub/Git
IDE: VSCode, STM32CubeIDE/CubeMX, ESP-IDF
Content: VSCode, Comment Anchor, Google Docs
-->

- [ ] Sắp xếp meeting với GV để thống nhất hướng đi và timeline.
- [ ] Thống nhất về các testobj và testbench cần thiết để thực hiện các thí nghiệm trong đồ án.
- [ ] Thống nhất về timeline và release selection của μEDP nhằm đảm bảo tính ổn định.
- [ ] Review lại phân công công việc trong Mục 6.4 để thống nhất với GV.
- [ ] Trình bày Sect 1, 2, 3 theo khung Mục 7 để GV duyệt trước khi thống nhất về bài toán AI.
- [ ] Dựng pipeline thực nghiệm sớm (đo baseline trước) để có số liệu xuyên suốt.
- [ ] Phân công 2 thành viên theo Mục 6.4 và thống nhất mốc thời gian với GV.

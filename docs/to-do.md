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
- [ ] Cập nhật nội dung các bài báo nghiên cứu cần thực hiện literature review trong tài liệu.
- [ ] Bổ sung lộ trình thiết kế API chi tiết cho thuật toán sẽ sử dụng.
- [ ] Cập nhật và sửa đổi PCB design với TCT40-16R thay bằng INMP441.

<!-- STATUS
Hiện tại đã sửa lại PCB design với Kicad v10 và bổ sung symbol + footprint bị thiếu cho các component trước khi thống nhất đưa ra review.
Việc đưa symbol INNP441 vào schematic sẽ sớm diễn ra trong thời gian tới.
-->

- [x] Bổ sung lại thông tin các bài báo nghiên cứu cần thực hiện literature review trong tài liệu.

<!-- STATUS
Hiện tại đã bổ sung ở `docs/references`
Tuy nhiên ở repo PdM-AF đã có sẵn các bài báo nghiên cứu cần thực hiện literature review trong tài liệu nên mở rộng hướng nghiên cứu để đạt một số lượng bài báo nghiên cứu cần thực hiện literature review trong tài liệu.

# TASK - Phân công nhiệm vụ cho các thành viên trong nhóm để thực hiện literature review và tổng hợp lại thành một tài liệu chung.
-->

- [ ] Bổ sung hoàn chỉnh các API cần thiết cho lớp PAL trên hardware chủ chốt STM32H723 và ESP32S3

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

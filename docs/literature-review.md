# Literature Review — CE201 (Predictive Maintenance via Acoustic Fingerprint on μEDP)

> Tài liệu này thực hiện task: Cập nhật nội dung các bài báo nghiên cứu cần thực hiện literature review
> trong tài liệu. Nội dung tổng hợp lại toàn bộ các bài báo nghiên cứu cần
> literature review cho đồ án, bao gồm 5 bài mới bổ sung ở `docs/references/` và các bài đã được khảo sát
> từ trước ở repo kế thừa [PdM-AF](https://github.com/1811htsang/PdM-AF), theo đúng ghi chú STATUS đã để
> lại: *"ở repo PdM-AF đã có sẵn các bài báo nghiên cứu cần thực hiện literature review trong tài liệu nên
> mở rộng hướng nghiên cứu để đạt một số lượng bài báo nghiên cứu cần thực hiện literature review"*.

## 1. Phạm vi & phương pháp tổng hợp

- **Nguồn 1 — `docs/references/` (CE201, mới bổ sung):** 5 bài báo tập trung vào TinyML/Edge AI cho
  predictive maintenance (PdM) và các bộ tăng tốc phần cứng cho anomaly detection.
- **Nguồn 2 — repo `PdM-AF` (kế thừa):** 11 bài báo đã được nhóm tiền nhiệm khảo sát khi thiết kế hệ thống
  thu thập dữ liệu âm thanh tốc độ cao trên ESP32-S3, xoay quanh acoustic fingerprinting và PdM bằng cảm
  biến nhúng.
- **Tổng cộng 16 bài báo** được đưa vào vòng literature review của CE201, đủ để lập bảng khảo sát cho
  Chương 2 (Cơ sở lý thuyết & khảo sát) theo khung Mục 7 của phiếu định hướng
  (`docs/review_planning_ce201.pdf`).
- Các bài được phân vào **5 nhóm chủ đề** để dễ tổng hợp và viết Chương 2, thay vì liệt kê rời rạc từng
  bài như hiện trạng cũ ở `docs/references/`.

## 2. Bảng tổng hợp tài liệu tham khảo

| # | Trích dẫn (rút gọn) | Năm | Nhóm chủ đề | Người phụ trách |
| --- | --- | --- | --- | --- |
| [1] | Gupta & Shivhare — Embedded TinyML for PdM (ESP32, 1D-CNN) | 2025 | A. TinyML/Edge AI runtime | Minh |
| [2] | Vermesan, Wotawa, Diaz Nava, Debaillie (Eds.) — Industrial AI Technologies and Applications | 2022 | E. Khảo sát/định vị tổng quan | Sang |
| [3] | Chen, Gao, Liang — LOPdM: self-powered sensing + TinyML | 2023 | A. TinyML/Edge AI runtime | Minh |
| [4] | Kolok, Hodoň, Ševčík, Hotz, Remy — Low-Cost IoT-Based PdM Using Vibration | 2025 | C. Kiến trúc IoT/nhúng chi phí thấp | Sang |
| [5] | Vitolo, De Vita, Di Benedetto, Pau, Licciardo — Low-Power In-Sensor Detection & Classification | 2022 | A. TinyML/Edge AI runtime (phần cứng) | Minh |
| [6] | Das, Borisov, Caesar — Do You Hear What I Hear? | 2014 | B. Acoustic sensing & fingerprinting nền tảng | Sang |
| [7] | Wang — Audio Data Acquisition System Design Based on ARM and DSP | 2015 | B. Acoustic sensing & fingerprinting nền tảng | Sang |
| [8] | Chuang, Sahoo, Lin, Chang — PdM with Sensor Data Analytics on Raspberry Pi | 2019 | D. Phương pháp luận & nền tảng phân tích PdM | Minh |
| [9] | Nunes, Santos, Rocha — Challenges in Predictive Maintenance: A Review | 2022 | E. Khảo sát/định vị tổng quan | Sang |
| [10] | Franco, de Figueiredo — Predictive Maintenance: An Embedded System Approach | 2022 | C. Kiến trúc IoT/nhúng chi phí thấp | Sang |
| [11] | Kumar & Paul — Device Fingerprinting for Cyber-Physical Systems: A Survey | 2023 | E. Khảo sát/định vị tổng quan | Sang |
| [12] | Omol, Mburu, Abuonji, Onyango — Anomaly Detection in IoT Sensor Data Using ML for PdM in Smart Grids | 2024 | A. TinyML/Edge AI runtime | Minh |
| [13] | García-Ortega — Application of Embedded Systems in Industrial IoT for PdM | 2024 | C. Kiến trúc IoT/nhúng chi phí thấp | Sang |
| [14] | Cummins, Sommers, Ramezani, Mittal, Jabour, Seale, Rahimi — Explainable PdM: A Survey | 2024 | E. Khảo sát/định vị tổng quan | Minh |
| [15] | Nagy & Lakatos — Acoustic Fingerprint in Vehicle Manufacturing | 2025 | B. Acoustic sensing & fingerprinting nền tảng | Minh |
| [16] | Arregi, Barrutia, Bediaga — End-to-End Methodology for PdM Based on Fingerprint Routines | 2025 | D. Phương pháp luận & nền tảng phân tích PdM | Minh |

> Cột "Người phụ trách" là đề xuất phân công ban đầu (chia đều theo nhóm chủ đề gắn với mắt xích sở hữu
> của mỗi thành viên trong README — Sang: runtime/hạ tầng đo đạc; Minh: pipeline AI/dataset).

## 3. Tổng hợp nội dung theo từng nhóm chủ đề

### A. TinyML / Edge AI runtime cho predictive maintenance — [1], [3], [5], [12]

Nhóm này khảo sát các hệ thống suy luận AI chạy trực tiếp trên vi điều khiển hoặc cảm biến, là nhóm liên
quan trực tiếp nhất tới Hướng A của đề tài. [1] xây dựng một mạng CNN 1 chiều nhỏ gọn, lượng tử hóa để
chạy trên ESP32, phân loại 4 trạng thái vận hành từ dữ liệu gia tốc kế 3 trục và đạt độ chính xác trên
90%; đây là tài liệu gần nhất về mặt kiến trúc mô hình với hướng CE201 đang chọn (CNN nhỏ + int8 + MCU).
[3] đề xuất một hệ PdM tự cấp nguồn kết hợp TinyML, so sánh nhiều mô hình học máy cổ điển (random forest,
DNN, …) trên dữ liệu rung động thu thập ở điều kiện lấy mẫu hạn chế, cho thấy độ đánh đổi giữa độ chính
xác và mức tiêu thụ năng lượng — hữu ích khi CE201 cần lập luận về lựa chọn giữa mô hình cây quyết định
nhẹ và mạng nơ-ron khi thiết kế benchmark. [5] đi xa hơn về phần cứng: đề xuất một bộ tăng tốc Auto-Encoder -
CNN được hiện thực trực tiếp trên silicon cùng MEMS sensor, minh họa giới hạn dưới về công suất/diện tích
mà một giải pháp phần mềm chạy trên MCU (như CE201) cần đối chiếu khi bàn luận trade-off. [12] khảo sát
việc áp dụng học máy cho phát hiện bất thường trên dữ liệu cảm biến IoT trong bối cảnh lưới điện thông
minh, cung cấp góc nhìn ứng dụng rộng hơn ngoài phạm vi rung động cơ khí thuần túy.

### B. Acoustic sensing & fingerprinting nền tảng — [6], [7], [15]

[6] là một trong những công trình nền tảng về acoustic fingerprinting của thiết bị điện tử thông qua thành
phần âm thanh nhúng (micro/loa), đặt cơ sở khái niệm cho việc dùng "dấu vân tay âm thanh" để nhận diện
trạng thái thiết bị — khái niệm mà tên đề tài CE201 (Acoustic Fingerprint) kế thừa trực tiếp. [7] trình bày
thiết kế phần cứng thu âm dựa trên ARM+DSP, liên quan tới lớp thu thập tín hiệu (I2S microphone) mà CE201
đang dùng lại từ repo PdM-AF. [15] là một ứng dụng gần đây của acoustic fingerprint trong sản xuất ô tô,
cho thấy tính khả thi thương mại của hướng tiếp cận này ngoài phạm vi phòng thí nghiệm.

### C. Kiến trúc IoT/nhúng chi phí thấp cho PdM — [4], [10], [13]

[4] là tài liệu gần nhất với hardware setup của CE201: hệ thống PdM chi phí thấp trên ESP32 kết hợp cả
accelerometer và microphone MEMS, xử lý bằng RMS/FFT rồi phân loại bất thường, đạt độ chính xác khoảng
73% — một baseline tham khảo hợp lý khi CE201 lập bảng so sánh độ chính xác mô hình của mình. [10] và [13]
khảo sát rộng hơn việc áp dụng hệ thống nhúng cho IIoT/PdM, cung cấp ngữ cảnh về các ràng buộc chi phí,
kết nối và triển khai thực tế mà CE201 cần đối chiếu khi viết Chương 1 (bối cảnh & động lực).

### D. Phương pháp luận & nền tảng phân tích PdM — [8], [16]

[8] trình bày một nền tảng thực nghiệm PdM dựa trên Raspberry Pi với phân tích dữ liệu cảm biến, là ví dụ
về pipeline "thu thập → phân tích → cảnh báo" ở quy mô lớn hơn MCU, hữu ích để đối chiếu chi phí/độ trễ.
[16] đề xuất một phương pháp luận đầu-cuối cho PdM dựa trên fingerprint routine kết hợp anomaly detection
cho các cụm trục quay của máy công cụ — gần nhất về mặt quy trình luận với pipeline "acoustic fingerprint →
anomaly detection" mà CE201 sẽ hiện thực.

### E. Khảo sát/định vị tổng quan — [2], [9], [11], [14]

Nhóm này không đề xuất hệ thống cụ thể mà cung cấp bức tranh tổng quan để CE201 định vị đóng góp của mình.
[2] là một tuyển tập chuyên khảo về công nghệ và ứng dụng AI công nghiệp, dùng để trích dẫn bối cảnh chung
của Industry 4.0/Industrial AI ở Chương 1. [9] tổng hợp các thách thức còn tồn đọng của PdM (dữ liệu nhãn
hiếm, drift, chi phí triển khai), là cơ sở để CE201 nêu rõ giới hạn/phạm vi ở Chương 1. [11] khảo sát rộng
về device fingerprinting cho hệ thống cyber-physical, giúp CE201 định vị "acoustic fingerprint" trong bức
tranh fingerprinting nói chung. [14] khảo sát về explainable PdM — một hướng mở rộng tiềm năng cho Chương 7
(hướng phát triển) nếu CE201 muốn bàn về khả năng diễn giải của mô hình sau khi lượng tử hóa.

## 4. Khoảng trống nghiên cứu & định vị đóng góp của CE201

Cả 16 bài báo trên đều tập trung vào **thuật toán AI hoặc kiến trúc phần cứng cảm biến** cho PdM, nhưng
không có bài nào bàn về **runtime lập lịch thời gian thực (Active Object/event-driven) làm nền chạy suy
luận AI xen kẽ các tác vụ real-time khác trên cùng một MCU** — đây chính là khoảng trống mà μEDP + CE201
lấp vào (đã được GVHD ghi nhận ở Mục 6.1 của `review_planning_ce201.pdf`: *"benchmark μEDP như một runtime
cho AI-at-edge"*). Vì vậy Chương 2 của báo cáo cần nhấn mạnh: các bài [1],[3],[4],[5] chứng minh tính khả
thi của việc chạy TinyML trên MCU tương tự (ESP32/STM32-class), nhưng đều triển khai trên bare-metal hoặc
vòng lặp polling đơn giản — chưa bài nào đối chiếu với một scheduler Active Object có đo đạc latency/jitter
tường minh như μEDP hướng tới.

## 5. Phân công nhiệm vụ

Theo TASK note đã ghi ở `docs/to-do.md` (*"Phân công nhiệm vụ cho các thành viên trong nhóm để thực hiện
literature review và tổng hợp lại thành một tài liệu chung"*), đề xuất phân công như sau — dựa theo mắt
xích sở hữu đã thống nhất ở README (Sang: runtime/benchmark, Minh: pipeline AI/dataset):

| Thành viên | Nhóm chủ đề phụ trách | Việc cần làm tiếp |
| --- | --- | --- |
| Huỳnh Thanh Sang | B, C, và phần "runtime/benchmark" của E | Đọc chi tiết + trích số liệu benchmark (nếu có) từ [4],[6],[7],[9],[10],[11],[13] để dùng cho Chương 2 & Chương 6 (so sánh baseline) |
| Nguyễn Hoàng Hải Minh | A, D, và phần "mô hình AI" của E | Đọc chi tiết kiến trúc mô hình + pipeline train/convert/deploy từ [1],[2],[3],[5],[8],[12],[14],[15],[16] để dùng cho Chương 4 (tích hợp AI) |

## 6. Tài liệu tham khảo

[1] S. Gupta and S. N. Shivhare, "Embedded TinyML for Predictive Maintenance: Vibration Analysis on ESP32
    with Real-Time Fault Detection in Industrial Equipment," *Int. J. Comput. Model. Appl.*, vol. 2, no. 2,
    pp. 1–17, Jun. 2025, doi: 10.63503/j.ijcma.2025.114.

[2] O. Vermesan, F. Wotawa, M. Diaz Nava, and B. Debaillie, Eds., *Industrial Artificial Intelligence
    Technologies and Applications*. Gistrup, Denmark: River Publishers, 2022, doi:
    10.1201/9781003377382.

[3] Z. Chen, Y. Gao, and J. Liang, "LOPdM: A Low-Power On-Device Predictive Maintenance System Based on
    Self-Powered Sensing and TinyML," *IEEE Trans. Instrum. Meas.*, vol. 72, Art. no. 2525213, 2023.

[4] P. Kolok, M. Hodoň, P. Ševčík, L. Hotz, and N. Remy, "Low-Cost IoT-Based Predictive Maintenance Using
    Vibration," *Sensors*, vol. 25, no. 21, Art. no. 6610, 2025, doi: 10.3390/s25216610.

[5] P. Vitolo, A. De Vita, L. Di Benedetto, D. Pau, and G. D. Licciardo, "Low-Power Detection and
    Classification for In-Sensor Predictive Maintenance Based on Vibration Monitoring," *IEEE Sensors J.*,
    vol. 22, no. 7, pp. 6942–6951, Apr. 2022, doi: 10.1109/JSEN.2022.3154479.

[6] A. Das, N. Borisov, and M. Caesar, "Do You Hear What I Hear?: Fingerprinting Smart Devices Through
    Embedded Acoustic Components," 2014.

[7] Y. Wang, "Audio Data Acquisition System Design Based on ARM and DSP," 2015.

[8] S.-Y. Chuang, N. Sahoo, H.-W. Lin, and Y.-H. Chang, "Predictive Maintenance with Sensor Data Analytics
    on a Raspberry Pi-Based Experimental Platform," 2019.

[9] P. Nunes, J. Santos, and E. Rocha, "Challenges in Predictive Maintenance – A Review," 2022.

[10] I. T. Franco and R. M. de Figueiredo, "Predictive Maintenance: An Embedded System Approach," 2022.

[11] V. Kumar and K. Paul, "Device Fingerprinting for Cyber-Physical Systems: A Survey," 2023.

[12] E. Omol, L. Mburu, P. Abuonji, and D. Onyango, "Anomaly Detection in IoT Sensor Data Using Machine
     Learning Techniques for Predictive Maintenance in Smart Grids," 2024.

[13] E. García-Ortega, "Application of Embedded Systems in Industrial IoT for Predictive Maintenance," 2024.

[14] L. Cummins, A. Sommers, S. Bakhtiari Ramezani, S. Mittal, J. Jabour, M. Seale, and S. Rahimi,
     "Explainable Predictive Maintenance: A Survey of Current Methods, Challenges and Opportunities," 2024.

[15] J. Nagy and I. Lakatos, "Acoustic Fingerprint in Vehicle Manufacturing as a Basis for Future
     Applications," 2025.

[16] A. Arregi, A. Barrutia, and I. Bediaga, "End-to-End Methodology for Predictive Maintenance Based on
     Fingerprint Routines and Anomaly Detection for Machine Tool Rotary Components," 2025.

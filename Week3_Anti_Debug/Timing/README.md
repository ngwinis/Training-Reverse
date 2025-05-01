# TIMING
Thực tế khi đang debug, luôn luôn có 1 khoảng delay khá lớn (so với thời gian chạy chương trình trung bình) giữa các câu lệnh và trình thực thi. Dựa vào cách tiếp cận này có thể phát hiện được chương trình đang bị debug hay không.

## **[1] RDPMC/RDTSC**
- `RDPMC` (Read Performance-Monitoring Counters) và `RDTSC` (Read Time-Stamp Counter) là các instruction x86 ASM được sử dụng để đo thời gian và hiệu suất của CPU, thường được sử dụng trong việc tối ưu hóa mã máy tính và phân tích hiệu suất hệ thống.
- 2 instructions này sử dụng cờ PCE trong thanh ghi CR4.
- `RDPMC` chỉ được sử dụng ở chế độ Kernel.
- `RDTSC` là lệnh được sử dụng ở chế độ User.
- Sau khi gọi 2 lệnh như trên, `RDPMC`/`RDTSC` đọc bộ đếm hiệu suất được chỉ định bởi `ECX` (ở đây là `ECX = 0` → bộ đếm 0), giá trị thời gian được lưu ở 2 thanh ghi `EDX:EAX`, với `EDX` lưu giá trị của 32bit cao và `EAX` lưu giá trị của 32bit thấp. (Xem thêm: [RDPMC](https://hjlebbink.github.io/x86doc/html/RDPMC.html))

  |Opcode*|Instruction|Op/En|64-Bit Mode|Compat/Leg Mode|Description|
  |-------|-----------|-----|-----------|---------------|-----------|
  |0F 33|RDPMC|NP|Valid|Valid|Read performance-monitoring counter specified by ECX into EDX:EAX.|

  > Code: [Anti-debug RDPMC](RDPMC/Anti-debug_RDPMC.cpp)

## **[2] `GetLocalTime()` & `GetSystemTime()`**
- Ý tưởng tổng quát: Debugger (đặc biệt là debug step-by-step hoặc breakpoints) thường làm chương trình chạy chậm hơn do phải dừng lại, ghi nhận trạng thái, phân tích từng dòng.</br>
--> Kỹ thuật Timing sử dụng `GetLocalTime()`/`GetSystemTime()` đo thời gian trôi qua giữa hai điểm trong chương trình để phát hiện bất thường bằng cách:
  - Ghi nhận thời gian trước và sau một đoạn code.
  - Nếu chênh lệch vượt ngưỡng cho phép thì sẽ nghi ngờ debugger.

- Hàm `GetLocalTime()`/`GetSystemTime()` nhận vào 1 đối số là con trỏ tới biến kiểu [LPSYSTEMTIME](https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-systemtime). Đây là 1 struct chứa tất cả các đơn vị thời gian (năm, tháng, ngày trong tuần, ngày, giờ, phút, giây, mili giây)

  ![alt text](../__images__/getlocaltime-1.png)

- Gọi hàm này 2 lần, mỗi lần truyền tham số kiểu `LPSYSTEMTIME` để truy xuất thời gian tại 2 thời điểm đó, sau đó tiếp tục truy xuất thời gian ra 1 struct [FILETIME](https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime) có 64-bit length.
- Sau khi đã truy xuất xong, tiến hành lấy hiệu giữa thời gian sau và thời gian trước rồi so sánh với "native" delay sẽ có thể nghi ngờ debugger đang chạy.

  > Code: 

## **[3] `GetTickCount()`**
- Hàm này có cùng ý tưởng với mục **[2]**, nhưng thay vì ghi thời gian vào biến thông qua truyền tham chiếu như `GetLocalTime()` hay `GetSystemTime()`, nó trả về trực tiếp một giá trị kiểu `DWORD`. Nhờ đó, ta có thể sử dụng ngay giá trị này để tính và so sánh thời gian trôi qua với "native" delay mà không cần xử lý trung gian.

  > Code: 
# PROCESS MEMORY
- Một process có thể kiểm tra bộ nhớ của chính nó để phát hiện sự hiện diện của trình gỡ lỗi hoặc can thiệp vào trình gỡ lỗi.

- Các kỹ thuật sau đây sẽ bao gồm bộ nhớ process và kiểm tra ngữ cảnh thread, tìm kiếm điểm dừng và vá chức năng như các phương pháp anti-attaching.

## **[1] Breakpoints**
- Ta luôn luôn có thể kiểm tra bộ nhớ tiến trình và tìm kiếm software breakpoint trong code hoặc kiểm tra các thanh ghi debug CPU để xác định xem hardware breakpoints đã được đặt hay chưa.

### **[1.1] Software Breakpoints (INT3)**
- Ý tưởng là xác định mã máy của một số hàm cho byte 0xCC, viết tắt của lệnh `INT 3`.
- Phương pháp này có thể tạo ra nhiều trường hợp "dương tính giả", do đó nên được sử dụng thận trọng.

  > Code mô phỏng: [Code C Software Breakpoints](antidebug_software_breakpoints.c)

### **[1.2] Hardware Breakpoints**
- Các thanh ghi DR0, DR1, DR2 và DR3 (vốn chỉ có thể được thiết lập bởi debugger như OllyDbg, x64dbg, WinDbg...) có thể được lấy từ ngữ cảnh thread. Nếu chúng chứa các giá trị khác không, điều đó có nghĩa là process được thực thi trong debugger và hardware breakpoints đã được đặt.

  > Code mô phỏng: [Code C Hardware Breakpoints](antidebug_hardware_breakpoint.c)

## **[2] Các kỹ thuật kiểm tra bộ nhớ khác**
### **[2.2] Detecting a Function Patch**
- Mục tiêu của kỹ thuật này là phát hiện xem hàm `IsDebuggerPresent()` có bị patch hoặc hook bởi trình gỡ lỗi hay không. Thay vì tin tưởng vào kết quả trả về của `IsDebuggerPresent()` – vốn dễ bị giả mạo – kỹ thuật này so sánh nội dung bộ nhớ của chính hàm đó giữa các tiến trình.
- Trong Windows, do DLL system-wide và ASLR chia sẻ base address cho từng phiên bản DLL (trong cùng một session), nên địa chỉ và nội dung hàm `kernel32!IsDebuggerPresent()` gần như giống nhau giữa các tiến trình.
- Nếu hàm trong tiến trình hiện tại bị sửa (patch hoặc inline hook), nội dung byte đầu sẽ khác với hàm tương ứng trong các tiến trình khác --> biết có sự can thiệp của debugger.

  > Code: [Code C Detect a function patch](detect_function_patch.c)
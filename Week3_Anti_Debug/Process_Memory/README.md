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


# INTERACTIVE CHECKS
Các kỹ thuật sau đây cho phép tiến trình đang chạy quản lý giao diện người dùng hoặc tương tác với tiến trình cha để phát hiện những điểm không nhất quán vốn có trong một tiến trình đã bị debug.

## **[1] Self-Debugging**
- `Self-debugging` là một kỹ thuật anti-debug dựa trên một đặc điểm của hệ điều hành Windows: Mỗi tiến trình chỉ được phép bị gỡ lỗi bởi một debugger tại một thời điểm.
- Cách hoạt động:
  - Một tiến trình (gọi là "phiên bản chính") sẽ tạo ra một tiến trình con (phiên bản thứ hai), truyền PID của nó làm đối số.
  - Phiên bản thứ hai cố gắng gọi `DebugActiveProcess()` để gắn debugger vào phiên bản chính.
  - Nếu không thể attach (thường là vì phiên bản chính đã bị debugger khác chiếm giữ), thì tiến trình thứ hai sẽ bật một "cờ báo hiệu" (event).
  - Quay lại ở phiên bản chính: nếu thấy event đã được bật, nó hiểu là có một debugger khác đang chạy.

  > Code: [Code C Anti-debug Self-Debugging](Self-Debugging/Anti-debug_Self-Debugging.c)

## **[2] `GenerateConsoleCtrlEvent()`**
- Khi người dùng nhấn tổ hợp `Ctrl+C` hoặc `Ctrl+Break` trong một cửa sổ console đang được focus, hệ điều hành Windows sẽ kiểm tra xem có trình xử lý nào đã đăng ký để tiếp nhận sự kiện này chưa. Mặc định, các tiến trình console sẽ xử lý bằng cách gọi `kernel32!ExitProcess()` để kết thúc tiến trình.
- Tuy nhiên, lập trình viên có thể đăng ký một hàm xử lý tùy chỉnh để bỏ qua hành vi mặc định khi nhận được tín hiệu `Ctrl+C` hoặc `Ctrl+Break`. Điều đặc biệt xảy ra khi chương trình console đang bị debug: nếu các tín hiệu `Ctrl+C` không bị vô hiệu hóa, Windows sẽ phát sinh ngoại lệ `DBG_CONTROL_C`.
- Thông thường, ngoại lệ này sẽ bị debugger chặn lại. Nhưng nếu chương trình tự thiết lập exception handler (trình xử lý ngoại lệ), nó có thể kiểm tra xem có nhận được `DBG_CONTROL_C` hay không. Nếu điều đó xảy ra mà debugger không chặn được, khả năng cao là chương trình đang bị debug. Đây chính là một kỹ thuật phát hiện debugger gián tiếp thông qua hành vi xử lý ngoại lệ.

  > Code: [Code Anti-debug GenerateConsoleCtrlEvent()](GenerateConsoleCtrlEvent/Anti-debug_GeenerateConsoleCtrlEvent.cpp)

## **[3] `BlockInput()`**
- Hàm `BlockInput()` trong thư viện user32.dll có khả năng chặn toàn bộ sự kiện từ chuột và bàn phím, khiến người dùng không thể tương tác với hệ thống. Điều này cũng có thể gây khó khăn cho debugger trong quá trình phân tích. Trên Windows Vista trở lên, việc sử dụng hàm này yêu cầu quyền quản trị viên (administrator).
- Ngoài tác dụng khóa input, hàm `BlockInput()` cũng có thể được sử dụng để phát hiện các công cụ gỡ lỗi hoặc hook đang can thiệp vào các API chống debugger. Theo thiết kế của hệ điều hành, `BlockInput(TRUE)` chỉ có thể gọi thành công một lần liên tiếp – lần gọi thứ hai sẽ trả về `FALSE`.
- Nếu cả hai lần gọi đều trả về `TRUE`, điều đó có thể là dấu hiệu cho thấy API đã bị can thiệp hoặc hook, chẳng hạn như bởi các công cụ phân tích động (debugger hoặc sandbox). Do đó, đây là một kỹ thuật gián tiếp để kiểm tra xem chương trình có bị theo dõi hay không.

  > Code: [Anti-debug BlockInput()](BlockInput/Anti-debug_BlockInput.cpp)
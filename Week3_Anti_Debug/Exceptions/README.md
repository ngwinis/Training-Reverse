# EXCEPTIONS
Các phương pháp sau đây cố tình gây ra ngoại lệ để xác minh xem hành vi tiếp theo có phải là bình thường đối với một tiến trình không bị debug hay không.
## **[1] `UnhandledExceptionFilter()`**
- Khi có một exception xảy ra và không có exception handler nào được đăng ký (hoặc được đăng ký nhưng không handle 1 exception nào) thì hàm `kernel32!UnhandledExceptionFilter()` sẽ được gọi.
  - Đây là một hàm do ứng dụng định nghĩa, nếu tiến trình đang bị debug, hàm này sẽ chuyển các ngoại lệ chưa được xử lý đến debugger. Ngược lại, nếu không bị debug, nó sẽ hiển thị thông báo "Application Error" và khiến trình xử lý ngoại lệ sẽ được thực thi.  
    ![alt text](../__images__/unhandleexceptionfilter-1.png)
  - Hàm này trả về 1 trong 2 giá trị như sau:

    | Mã trả về | Mô tả |
    |-----------|-------|
    |`EXCEPTION_CONTINUE_SEARCH`</br>0x00|Tiến trình đang bị debug, do đó ngoại lệ phải được chuyển tới debugger|
    |`EXCEPTION_EXECUTE_HANDLER`</br>0x1|Nếu flag `SEM_NOGPFAULTERRORBOX` được chỉ định trong lệnh gọi `SetErrorMode` trước đó, không có thông báo Application Error được hiển thị. Hàm trả lại quyền điều khiển cho trình xử lý ngoại lệ|

- Thay vì xử lý ngoại lệ theo mặc định, chúng ta có thể đăng ký một unhandled exception filter tự custom sử dụng hàm `kernel32!SetUnhandledExceptionFilter()`. Cách hoạt động của hàm này tương đối đơn giản. Mặc định nếu không bị debug thì thông báo "Application Error" được hiển thị, còn hàm kia sẽ giúp chúng ta hiển thị 1 thông báo tuỳ chỉnh theo ý muốn.
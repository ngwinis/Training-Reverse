# ASSEMBLY INSTRUCTIONS
Các kỹ thuật sau đây nhằm mục đích phát hiện sự hiện diện của trình gỡ lỗi dựa trên cách mà trình gỡ lỗi phản ứng khi CPU thực thi một số lệnh nhất định.

## **[1] INT 3**
- Lệnh `INT3` là một lệnh ngắt được sử dụng như một software breakpoint. Nếu không có debugger, sau khi đến lệnh `INT3`, ngoại lệ `EXCEPTION_BREAKPOINT` (0x80000003) được tạo ra và trình xử lý ngoại lệ sẽ được gọi. Nếu có debugger, quyền điều khiển sẽ không được trao cho trình xử lý ngoại lệ.
  
  > Code: [Code Anti-debug INT3](Anti-debug_INT3/int3.c)

- Ngoài dạng ngắn phổ biến của lệnh `INT 3` (với mã máy là `0xCC`), còn tồn tại một dạng dài của lệnh này với opcode là `CD 03`.
- Khi xảy ra ngoại lệ `EXCEPTION_BREAKPOINT`, Windows sẽ tự động giảm thanh ghi EIP (hoặc RIP trên hệ 64-bit) để trỏ về vị trí giả định của byte `0xCC` và chuyển quyền điều khiển cho trình xử lý ngoại lệ. Tuy nhiên, nếu sử dụng dạng dài của lệnh `INT 3`, lúc này EIP sẽ trỏ vào giữa lệnh — cụ thể là byte `0x03`. Điều này khiến quá trình tiếp tục thực thi sau khi xử lý ngoại lệ trở nên không chính xác.
- Vì vậy, nếu muốn chương trình tiếp tục chạy bình thường sau khi gặp `INT 3` (dạng dài), trình xử lý ngoại lệ cần hiệu chỉnh lại giá trị của EIP để bỏ qua toàn bộ lệnh `CD 03`. Nếu không thực hiện điều chỉnh này, rất có thể chương trình sẽ gặp lỗi `EXCEPTION_ACCESS_VIOLATION` do CPU cố gắng thực thi tại vị trí không hợp lệ. Tuy nhiên, nếu không có ý định tiếp tục thực thi sau đó, ta có thể bỏ qua việc điều chỉnh con trỏ lệnh.
  
  > Code: [Code C++ Anti-debug Long form](Anti-debug_Long-form/anti-debug_long-form.cpp)

## **[2] INT 2D**
- Tương tự như lệnh INT 3, lệnh INT 2D khi được thực thi cũng gây ra ngoại lệ EXCEPTION_BREAKPOINT. Tuy nhiên, có một số điểm khác biệt quan trọng:
  - Khi INT 2D được gọi, Windows sẽ sử dụng giá trị của thanh ghi EIP tại thời điểm đó làm địa chỉ ngoại lệ, sau đó tự động tăng giá trị EIP lên.
  - Windows cũng kiểm tra giá trị của thanh ghi EAX tại thời điểm gọi INT 2D. Nếu EAX có giá trị 1, 3, 4 (trên mọi phiên bản Windows) hoặc 5 (trên Windows Vista trở lên) thì EIP sẽ được tăng thêm 1 byte nữa.
  - Việc thay đổi EIP như vậy có thể làm cho các debugger không xử lý chính xác quá trình khôi phục luồng thực thi, dẫn đến việc bỏ qua byte tiếp theo sau INT 2D, từ đó gây lỗi hoặc "vượt qua" đoạn mã có chủ đích.
- Để tránh điều đó, một byte NOP (không làm gì) được chèn sau INT 2D. Nhờ đó, dù có bị bỏ qua hay không, việc thực thi chương trình sẽ không bị phá vỡ.
- Nếu chương trình không bị gỡ lỗi, trình xử lý ngoại lệ sẽ được gọi và quá trình xử lý diễn ra như dự kiến. Nếu chương trình đang bị gỡ lỗi, debugger có thể xử lý INT 2D khác đi (hoặc gây lỗi), và quá trình kiểm tra anti-debug có thể phát hiện điều này.

  > Code: [Code Anti-debug INT 2D](Anti-debug_INT2D/Anti-debug_INT2D.cpp)

## **[3] DebugBreak**
- Theo tài liệu chính thức của hàm [DebugBreak](https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-debugbreak), lệnh này sẽ gây ra một ngoại lệ kiểu `EXCEPTION_BREAKPOINT` trong tiến trình hiện tại. Điều này cho phép luồng gọi hàm phát tín hiệu đến debugger để xử lý ngoại lệ này.
- Nếu chương trình đang chạy bình thường (không có debugger), ngoại lệ `EXCEPTION_BREAKPOINT` sẽ được truyền đến handler và có thể xử lý theo ý muốn.
- Ngược lại, nếu chương trình đang chạy trong môi trường có debugger, debugger sẽ chặn và xử lý ngoại lệ này thay vì để chương trình tự xử lý. Điều này làm luồng thực thi không chuyển vào khối `__except`, từ đó có thể nhận diện sự hiện diện của debugger.

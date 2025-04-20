# OBJECT HANDLES
## **[1] OpenProcess()**
- Một số debuggers có thể bị detect với hàm `OpenProcess()`.
- Cơ chế hoạt động của dạng anti-debug này là chương trình sẽ cố gắng mở process `csrss.exe` bằng `OpenProcess()`. Process này là một tiến trình hệ thống, thông thường chỉ có thể mở với quyền Admin. Mặt khác, khi **bị debug**, chỉ có quyền `SeDebugPrivilege` được bật bởi 1 số debugger thì mới thành công, ngược lại nó sẽ mặc định bị tắt.
- Điều đó có nghĩa là nếu tiến trình `csrss.exe` được mở thành công bởi `OpenProcess()` (nói cách khác, `OpenProcess()` trả về giá trị khác `NULL`) thì trình debugger sẽ bị phát hiện.
- Tuy nhiên, nếu không debug nhưng lại chạy dưới quyền Admin thì hàm `OpenProcess()` vẫn không thành công do không được bật quyền `SeDebugPrivilege`.
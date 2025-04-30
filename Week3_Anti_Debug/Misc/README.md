# MISC
## **[1] `FindWindow()`**
- Một trong những kỹ thuật anti-debug đơn giản nhưng hiệu quả là dò tìm các cửa sổ (window) thuộc về debugger đang hoạt động. Kỹ thuật này hoạt động bằng cách liệt kê các lớp cửa sổ (window classes) trong hệ thống và so sánh với các tên lớp cửa sổ đã biết của các trình gỡ lỗi phổ biến.
- Một số API thường được dùng:
  - `FindWindowW()` hoặc `FindWindowA()`
  - `FindWindowExW()` hoặc `FindWindowExA()`
- Các API này thuộc thư viện `user32.dll`, cho phép kiểm tra sự tồn tại của một cửa sổ với tên lớp (class name) hoặc tiêu đề (window name). Nếu một cửa sổ thuộc về debugger tồn tại trong hệ thống, ta có thể xác định chương trình đang bị debug.

  > Code: [Code C++ Anti-debug FindWindow()](FindWindow/Anti-debug_FindWindow.cpp)

## **[2] Parent Process Check**
- Thông thường, một tiến trình chế độ người dùng được thực hiện bằng cách nhấp đúp vào biểu tượng tệp. Nếu tiến trình được thực hiện theo cách này, tiến trình cha của nó sẽ là tiến trình shell (`explorer.exe`).
- Ý tưởng chính của hai phương pháp sau là so sánh PID của tiến trình cha với PID của `explorer.exe`.

### **[2.1] `NtQueryInformationProcess()`**
- Kỹ thuật này nhằm phát hiện debugger bằng cách kiểm tra ID tiến trình cha (Parent Process ID) của tiến trình hiện tại. Nếu chương trình không được khởi chạy từ Windows Explorer (shell mặc định), mà từ một tiến trình khác (ví dụ như debugger), thì có khả năng đang bị debug.
- Nguyên lý hoạt động:
  - Lấy handle cửa sổ Explorer (shell) qua `GetShellWindow()` – đây là cửa sổ giao diện chính của hệ điều hành (File Explorer).
  - Lấy PID của Explorer bằng `GetWindowThreadProcessId()`.
  - Lấy thông tin tiến trình hiện tại, bao gồm `InheritedFromUniqueProcessId` (PID cha), bằng cách gọi `NtQueryInformationProcess()` từ ntdll.dll.
  - So sánh PID cha của tiến trình hiện tại với PID của Explorer. Nếu khác nhau, có thể chương trình đang bị khởi chạy từ một debugger.

  > Code: [Code C++ Anti-debug NtQueryInformationProcess()](NtQueryInformationProcess/Anti-debug_NtQueryInformationProcess.cpp)

### **[2.2] `CreateToolhelp32Snapshot()`**
- Kỹ thuật anti-debug này sử dụng các hàm trong Windows API như `CreateToolhelp32Snapshot()` và `Process32Next()` để lấy thông tin về tất cả tiến trình đang chạy trong hệ thống.
- Sau khi lấy snapshot, chương trình có thể duyệt qua danh sách tiến trình và tìm ra tiến trình cha (parent process) của chính nó.
- Từ đó, có thể kiểm tra tên tiến trình cha (ví dụ: `explorer.exe` là bình thường, còn `x64dbg.exe`, `ida.exe`, `cmd.exe`,... có thể là dấu hiệu của debugger).
- Nếu phát hiện tiến trình cha là công cụ debug hoặc khác thường, chương trình sẽ nghi ngờ rằng nó đang bị debug.

  > Code: [Code C++ Anti-debug CreateToolhelp32Snapshot()](CreateToolhelp32Snapshot/Anti-debug_CreateToolhelp32Snapshot.cpp)
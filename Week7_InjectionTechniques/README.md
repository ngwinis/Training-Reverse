# Các kỹ thuật Inject trong Reverse

Trong Reverse Engineering và malware analysis, Injection thường ám chỉ các kỹ thuật `code injection` hay `process injection` - tức là đưa một đoạn code (thường là shellcode, dll, hoặc payload) vào một tiến trình khác hoặc vào tiến trình đang chạy. Có nhiều kỹ thuật phổ biến, được chia thành các nhóm như sau:

## **[1] DLL Injection**
- DLL Injection là kỹ thuật cho phép kẻ tấn công nạp một thư viện DLL vào tiến trình hợp pháp để thực thi mã độc trong ngữ cảnh của tiến trình đó. Cách phổ biến là ghi đường dẫn DLL vào bộ nhớ tiến trình đích (dùng `VirtualAllocEx`, `WriteProcessMemory`), rồi tạo luồng mới (`CreateRemoteThread`) gọi `LoadLibrary` để nạp DLL.
- Dll injection có nhiều biến thể:
    - **Reflective/Manual Mapping**: DLL tự nạp hoặc được ánh xạ thủ công, không cần `LoadLibrary`.
    - **DLL Hollowing / Module Stomping**: nạp DLL hợp lệ, sau đó ghi đè entry point bằng mã độc để che giấu.
- Nhờ chạy trong tiến trình hợp pháp, DLL injection giúp kẻ tấn công truy cập tài nguyên, tăng đặc quyền và né tránh nhiều cơ chế phòng thủ.

> Demo: [Classic Dll Injection](Demo/01_Dll_Injection/Dll_Injection)

> Demo: [Reflective Dll Injection](Demo/01_Dll_Injection/Reflective_Dll_Injection/)

## **[2] PE Injection**
- PE Injection là kỹ thuật nạp một tệp thực thi (PE, có thể không tồn tại trên đĩa) vào bộ nhớ của tiến trình hợp pháp để thực thi trong ngữ cảnh của tiến trình đó. Quá trình thường gồm: cấp phát vùng nhớ (`VirtualAllocEx`), ghi dữ liệu PE vào (`WriteProcessMemory`), rồi kích hoạt bằng `CreateRemoteThread` hoặc shellcode. Vì PE cần ánh xạ lại header, section và relocation, nên kỹ thuật này phức tạp hơn tiêm shellcode.
- Khi chạy trong tiến trình hợp pháp, PE injection cho phép kẻ tấn công tận dụng bộ nhớ, tài nguyên hệ thống, thậm chí nâng cao đặc quyền, đồng thời né tránh nhiều giải pháp bảo mật do mã độc được ẩn dưới tiến trình hợp lệ.

## **[3] Thread Execution Hijacking**
- Thread Execution Hijacking là kỹ thuật chiếm quyền điều khiển một luồng đang chạy trong tiến trình hợp pháp để thực thi mã độc. Kẻ tấn công thường tạm dừng luồng (`SuspendThread`), cấp phát và ghi mã mới (`VirtualAllocEx`, `WriteProcessMemory`), chỉnh lại context (`SetThreadContext`), rồi tiếp tục luồng (`ResumeThread`). Cách này cho phép chạy mã trong tiến trình hợp lệ, tận dụng tài nguyên, có thể nâng cao đặc quyền và né tránh phát hiện của các cơ chế bảo mật.

## **[4] Asynchronous Procedure Call**
- APC Injection là kỹ thuật kẻ tấn công chèn mã độc vào tiến trình thông qua hàng đợi Asynchronous Procedure Call (APC) nhằm né tránh cơ chế phòng thủ dựa trên tiến trình và có thể leo thang đặc quyền. Cách làm thường là lấy handle của luồng mục tiêu (như qua `OpenThread`), rồi dùng `QueueUserAPC` gọi hàm độc hại (ví dụ `LoadLibraryA` nạp DLL).
- Các biến thể gồm:
    - **Early Bird Injection**: tiêm mã vào tiến trình mới tạo ở trạng thái suspend trước khi entry point chạy, qua mặt anti-malware hook.
    - **AtomBombing**: ghi mã vào global atom table rồi kích hoạt bằng APC.
- Thực thi mã trong bối cảnh tiến trình hợp pháp giúp kẻ tấn công truy cập bộ nhớ, tài nguyên hệ thống/mạng, có thể nâng quyền, đồng thời khó bị phát hiện do hoạt động được che giấu dưới tiến trình hợp lệ.

## **[5] Thread Local Storage**
- TLS Callback Injection là kỹ thuật kẻ tấn công lợi dụng Thread Local Storage (TLS) callbacks để thực thi mã độc trong tiến trình khác, nhằm né tránh cơ chế phòng thủ và có thể leo thang đặc quyền.
- Cách làm là chỉnh sửa con trỏ trong file PE để chuyển hướng thực thi sang mã độc trước khi tới entry point hợp pháp. TLS callbacks vốn được hệ điều hành dùng để khởi tạo/dọn dẹp dữ liệu cho luồng, nhưng có thể bị thao túng bằng các kỹ thuật tiêm khác như Process Hollowing.
- Nhờ chạy dưới ngữ cảnh tiến trình hợp pháp, mã độc có thể truy cập bộ nhớ, tài nguyên hệ thống/mạng, thậm chí nâng quyền, đồng thời khó bị phát hiện do bị che giấu dưới tiến trình bình thường.

## **[6] Ptrace System Calls**
- Ptrace Injection lợi dụng system call ptrace để tiêm mã độc vào tiến trình đang chạy, nhằm né tránh phòng thủ và có thể leo thang đặc quyền.
- Kỹ thuật này gắn vào tiến trình mục tiêu rồi thay đổi bộ nhớ và thanh ghi của nó. Mã độc thường được ghi vào vùng nhớ mới (ví dụ `malloc`), sau đó thực thi bằng cách chỉnh thanh ghi lệnh với `PTRACE_SETREGS`, hoặc ghi trực tiếp bằng `PTRACE_POKETEXT`/`PTRACE_POKEDATA`.
- Ptrace injection thường chỉ khả thi với tiến trình con hoặc tiến trình có cùng/ít đặc quyền hơn. Nhờ chạy trong tiến trình hợp pháp, mã độc có thể truy cập bộ nhớ, tài nguyên hệ thống/mạng và khó bị phát hiện bởi phần mềm bảo mật.

## **[7] Proc Memory**
- Proc Memory Injection lợi dụng hệ thống tệp /proc để tiêm mã độc vào tiến trình đang chạy, giúp né tránh phòng thủ dựa trên tiến trình và có thể leo thang đặc quyền.
- Kỹ thuật này truy xuất /proc/[pid] để đọc ánh xạ bộ nhớ, tìm các gadget/đoạn lệnh rồi xây dựng payload ROP. Kẻ tấn công có thể ghi đè stack hoặc các vùng nhớ mục tiêu thông qua /proc/[pid]/maps (ví dụ dùng dd), từ đó vượt qua cơ chế bảo vệ như ASLR. Ngoài ra, có thể kết hợp với kỹ thuật khác (như Dynamic Linker Hijacking) để bổ sung gadget, hoặc áp dụng lên tiến trình con (ví dụ sleep).
- Chạy mã trong tiến trình hợp pháp cho phép truy cập bộ nhớ, tài nguyên hệ thống/mạng và ẩn lẫn dưới tiến trình hợp lệ, khiến việc phát hiện trở nên khó khăn.

## **[8] Extra Window Memory Injection**
- EWM Injection lợi dụng Extra Window Memory (EWM) trong tiến trình Windows để tiêm mã độc, né tránh cơ chế phòng thủ và có thể leo thang đặc quyền.
- Trước khi tạo cửa sổ, tiến trình cần đăng ký window class, trong đó có thể cấp tối đa 40 byte EWM cho mỗi instance. Dù nhỏ, vùng nhớ này đủ để chứa một con trỏ 32-bit, thường trỏ tới window procedure. Mã độc có thể ghi payload vào vùng nhớ chia sẻ của tiến trình, lưu con trỏ đến payload trong EWM, rồi kích hoạt thực thi bằng cách điều hướng luồng xử lý đến địa chỉ đó.
- Kỹ thuật này tránh được các API giám sát chặt chẽ như `WriteProcessMemory` hay `CreateRemoteThread`, đồng thời có thể kết hợp các thủ thuật để vượt DEP. Việc thực thi trong tiến trình hợp pháp giúp kẻ tấn công truy cập bộ nhớ, tài nguyên hệ thống/mạng và ẩn lẫn khỏi phần mềm bảo mật.

## **[9] Process Hollowing**
- Process Hollowing là kỹ thuật tạo tiến trình ở trạng thái tạm dừng, sau đó làm rỗng (`unmap`) bộ nhớ và thay thế bằng mã độc. Việc này thường được thực hiện qua API Windows như `CreateProcess` (tạo tiến trình tạm dừng), rồi `ZwUnmapViewOfSection`/`NtUnmapViewOfSection` để xóa bộ nhớ, sau đó ghi payload mới bằng `VirtualAllocEx`, `WriteProcessMemory`, `SetThreadContext` và khởi động lại bằng `ResumeThread`.
- Kỹ thuật này tương tự TLS Injection nhưng thay vì nhắm tiến trình có sẵn thì tạo tiến trình mới. Dù không giúp leo thang đặc quyền (tiến trình tiêm kế thừa quyền của tiến trình cha), nó vẫn khó bị phát hiện vì chạy ẩn dưới tiến trình hợp pháp.

## **[10] Process Doppelganging**
- Process Doppelganging là kỹ thuật tiêm mã độc vào tiến trình để né tránh cơ chế phòng thủ và che giấu thực thi. Nó lợi dụng Windows Transactional NTFS (TxF) – một tính năng cho phép thao tác tệp trong giao dịch (có thể ghi đè rồi rollback mà không để lại dấu vết).
- Tương tự Process Hollowing, kỹ thuật này thay thế bộ nhớ của tiến trình hợp pháp bằng mã độc, nhưng khác ở chỗ nó tận dụng TxF nên không cần gọi các API thường bị giám sát như `NtUnmapViewOfSection`, `VirtualProtectEx`, `SetThreadContext`.
- Quy trình gồm 4 bước:
    - Transact – Mở giao dịch TxF trên file hợp pháp rồi ghi đè bằng mã độc.
    - Load – Nạp mã độc vào bộ nhớ chia sẻ.
    - Rollback – Hoàn tác thay đổi, xóa dấu vết trên file gốc.
    - Animate – Tạo tiến trình từ vùng nhớ đã bị thay thế và khởi chạy.
- Mặc dù không giúp leo thang đặc quyền (tiến trình con vẫn kế thừa quyền của tiến trình cha), kỹ thuật này vẫn nguy hiểm vì che giấu được dưới tiến trình hợp pháp và khó bị phát hiện.

## **[11] VDSO Hijacking**
- VDSO Hijacking là kỹ thuật tiêm mã độc bằng cách lợi dụng Virtual Dynamic Shared Object (VDSO) để né tránh cơ chế phòng thủ tiến trình và có thể leo thang đặc quyền.
- Kỹ thuật này thay vì ghi trực tiếp mã thực thi (có thể bị chặn bởi cơ chế bảo vệ bộ nhớ), kẻ tấn công chiếm quyền stub syscall trong vdso để gọi hệ thống, từ đó nạp và ánh xạ một thư viện độc hại. Dòng thực thi sau đó được chuyển hướng thông qua việc sửa đổi Global Offset Table (GOT), nơi lưu địa chỉ tuyệt đối của các hàm thư viện.
- Thực thi mã trong tiến trình hợp pháp giúp kẻ tấn công truy cập bộ nhớ, tài nguyên hệ thống/mạng và khó bị phát hiện vì hành vi ẩn dưới tiến trình hợp pháp.

## **[12] ListPlanting**
- ListPlanting là kỹ thuật tiêm mã độc vào tiến trình thông qua list-view controls (`SysListView32`), giúp né tránh cơ chế phòng thủ và che giấu thực thi dưới tiến trình hợp pháp.
- Kỹ thuật này hoạt động bằng cách chép mã vào bộ nhớ của tiến trình có list-view control, rồi lợi dụng callback sắp xếp (`ListView_SortItems`) để thực thi mã độc. Việc sao chép mã có thể thực hiện trực tiếp qua handle cửa sổ (`FindWindow`, `EnumWindows`) hoặc qua các kỹ thuật injection khác.
- Một số biến thể còn dùng window messages (`PostMessage`/`SendMessage`) để ghi payload từng phần nhỏ, tránh bị giám sát bởi `WriteProcessMemory`. Sau cùng, payload được kích hoạt khi gửi thông điệp `LVM_SORTITEMS`.
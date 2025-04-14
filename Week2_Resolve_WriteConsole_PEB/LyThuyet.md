# RESOLVE ĐỊA CHỈ - NGUYÊN LÝ HOẠT ĐỘNG
## **[1] Các bước thực hiện**
Quá trình resolve địa chỉ hàm thủ công là một kỹ thuật quan trọng trong lập trình Assembly cấp thấp, đặc biệt trong viết shellcode hay phần mềm độc lập không phụ thuộc vào loader của hệ điều hành. Hãy tìm hiểu các bước từ khi chương trình được tải vào bộ nhớ:
### **1. Tải chương trình vào bộ nhớ**
Khi một chương trình được tải vào bộ nhớ, hệ điều hành Windows thực hiện các bước sau:
- Tạo một process mới
- Cấp phát không gian bộ nhớ ảo (virtual memory)
- Đọc file PE vào bộ nhớ
- Nạp các DLL cần thiết (bao gồm `kernel32.dll`, `ntdll.dll`...)
- Thiết lập cấu trúc dữ liệu PEB (Process Environment Block)
- Điều chỉnh các giá trị trong thanh ghi đoạn, bao gồm `FS` để trỏ đến TEB (Thread Environment Block)

### **2. Cấu trúc PEB và module loading**
PEB chứa thông tin quan trọng về process, trong đó có danh sách các modules đã được nạp:
- Tại `offset 0x30` trong TEB có địa chỉ của PEB
- Trong PEB tại `offset 0x0C` có con trỏ đến `PEB_LDR_DATA` (Loader Data)
- `PEB_LDR_DATA` chứa ba danh sách liên kết của các modules: `InLoadOrderModuleList`, `InMemoryOrderModuleList`, và `InInitializationOrderModuleList`

### **3. Tìm base address của kernel32.dll**
Đoạn code assembly sau sẽ lấy base address của kernel32.dll:
```
mov eax, fs:[30h]            ; Lấy địa chỉ PEB từ TEB
mov eax, [eax+0Ch]           ; Lấy Ldr từ PEB
mov eax, [eax+14h]           ; Lấy InMemoryOrderModuleList
mov eax, [eax]               ; Lấy entry thứ nhất (ntdll.dll)
mov eax, [eax]               ; Lấy entry thứ hai (kernel32.dll)
mov eax, [eax+10h]           ; Lấy BaseAddress của kernel32.dll
```

### **4.Cấu trúc file PE và ExportTable**
Khi đã có base address của `kernel32.dll`, chương trình cần tìm hàm `GetProcAddress` bằng cách:
- Truy cập PE Header
- Tìm Export Directory
- Truy cập Export Table

### **5. Tìm `GetProcAddress` bằng cách duyệt danh sách tên hàm**
Bây giờ chương trình sẽ duyệt qua danh sách tên hàm để tìm "GetProcAddress":
- Duyệt qua từng tên hàm trong danh sách
- So sánh với chuỗi "GetProcAddress"
- Khi tìm thấy, lấy ordinal tương ứng
- Sử dụng ordinal để tìm RVA của hàm từ `AddressOfFunctions`
- Cộng RVA với địa chỉ base để có địa chỉ thực của hàm

### **6.Sử dụng `GetProcAddress` để resolve các hàm khác**
Khi đã có hàm `GetProcAddress`, việc resolve các hàm còn lại trở nên đơn giản, bằng cách truyền vào các tham số tương ứng như được mô tả trên trang chủ của Microsoft:
```
FARPROC GetProcAddress(
  [in] HMODULE hModule,
  [in] LPCSTR  lpProcName
);
```
### **7. Tiếp tục thực thi chương trình**
Sau khi đã resolve tất cả hàm cần thiết, chương trình có thể gọi các hàm này để thực hiện các tác vụ mà người lập trình mong muốn

## **[2] TÀI LIỆU THAM KHẢO**
- [PE file](https://phamcongit.wordpress.com/2017/07/06/tim-hieu-ve-pe-file-p1/)
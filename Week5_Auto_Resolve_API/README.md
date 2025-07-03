# AUTO RESOLVE API

## [1] Đặt vấn đề

- Trong bài reverse [anti3](anti3), có 1 số đoạn có mã hex như sau:
    ![alt text](images/1.png)

### **1.1 Mã hash**
- Khi debug, đặt breakpoint tại lệnh `call` và kiểm tra thanh ghi `eax` xem giá trị trả về thì ta có thể thấy 1 số string khá quen thuộc như sau:

    ![alt text](images/2.png)
    ![alt text](images/3.png)

- Thực tế các đoạn mã hex đó chính là mã hash crc32 của 1 số hàm WindowsAPI và tên của 1 số dll thường được sử dụng. Đây là 1 trong những kỹ thuật obfuscate điển hình của anti-debug.

### **1.2 Các hàm resolve địa chỉ**
- Hàm `sub_401DF0()` đổi tên thành hàm `resolve_dll()`
- Hàm `sub_401F10()` đổi tên thành hàm `resolve_func()`
    
**[+] Hàm resolve_dll()**
    ![alt text](images/4.png)
- Mục tiêu của hàm này là lấy địa chỉ của 1 dll cần tìm
- Nó duyệt toàn bộ dll được nạp vào chương trình rồi so sánh với mã hash truyền vào

**[+] Hàm resolve_func()**
    ![alt text](images/5.png)
- Mục tiêu của hàm này là lấy địa chỉ của 1 hàm nằm trong file dll vừa lấy từ hàm `resolve_dll()`
- Nó duyệt toàn bộ các hàm nằm trong bảng export directory của file dll trên rồi so sánh với mã hash truyền vào

## [2] Hướng giải quyết:
### **2.1 hashdb**
- Để giải quyết bài toán liên quan đến mã hash như trên, ta có thể sử dụng cách đơn giản là tìm kiếm mã hash tương ứng đã được lưu trong CSDL rồi ánh xạ nó với tên dll hay tên hàm tương ứng.
- Một CSDL hỗ trợ cách làm trên chính là [hashdb](https://github.com/OALabs/hashdb).
- Trong IDA, có thể cài đặt 1 plugin có các tính năng dự đoán mã hash và hỗ trợ **query trực tiếp** các mã hash trên CSDL, đó là [hashdb-ida](https://github.com/OALabs/hashdb-ida). Chi tiết cách cài đặt đều có trong link đính kèm.
- Trong 1 chương trình có thể có rất nhiều mã hash, việc thực hiện kiểm tra thủ công từng mã hash sẽ rất mất thời gian, vì thế chúng ta có thể sử dụng một plugin khác hỗ trợ giải quyết vấn đề này bằng cách **query hàng loạt**, đó là [Shellcode Hashes](https://github.com/mandiant/flare-ida/tree/master). Chi tiết cách cài đặt có trong link đính kèm.

### **2.2 Thực hành trên file anti3.exe**
**[+] Thực hiện query trực tiếp**
- Ví dụ ở hàm xử lý case 2 (hàm `func_case2()`) của luồng chính có đoạn tương tự như sau:
    
    ![alt text](images/6.png)

- Đặt con trỏ chuột tại mã hash `6AE69F02h` --> chuột phải --> click chọn `HashDB Hunt Algorithm` để tìm kiếm loại hash khả quan

    ![alt text](images/7.png)
    ![alt text](images/9.png)

- Tương tự, để tìm kiếm bản plaintext của mã hash đó, ta chọn `HashDB Lookup`

    ![alt text](images/8.png)
    ![alt text](images/10.png)

**[+] Thực hiện query hàng loạt**
- Chọn `Edit` --> `Plugins` --> `Shellcode Hashes`, 1 bảng chọn database hiện ra, chọn database đi kèm trong hướng dẫn `Shellcode Hashes` được đề cập ở trên, đường dẫn của nó là `flare-ida\shellcode_hashes\sc_hashes.db`, chọn thuật toán hash `crc32`, bỏ tick `Use XOR seed` và để ý bảng Output khi nó thực hiện xong.

    ![alt text](images/11.png)
    ![alt text](images/12.png)
    ![alt text](images/13.png)

- Ví dụ kết quả sau khi đã sử dụng plugin trên có thể thấy ở những chỗ có mã hash đều hiện comment về hàm WindowsAPI tương ứng:

    ![alt text](images/14.png)

## [3] Vấn đề phát sinh
- Có thể thấy ở ảnh phía trên, chỉ có mỗi hàm `kernel32.dll!GetVersion()` được resolve, ngay tại dòng 19, hàm `resolve_func()` cũng được gọi 1 lần nữa nhưng không có API nào được resolve. Thậm chí khi sử dụng cách trực tiếp resolve thì cũng không tìm được plaintext tương ứng.

    ![alt text](images/15.png)

- Khi kiểm tra lại hàm `resolve_dll()` hoặc `resolve_func()`, thấy có 1 đoạn rẽ nhánh so sánh mã hash có sử dụng hàm `abs32(~v7)`.

    ![alt text](images/16.png)

- Mục đích của hàm này là lấy giá trị dương của kết quả phép toán not bit.
- Để xử lý vấn đề này, ta cần phải lấy giá trị not bit ngược lại rồi thực hiện cách query hash trực tiếp. Khi này kết quả hash sẽ có thể tìm được trong database.
- Cách làm là chuột phải tại mã hash đó --> click `-<giá trị hash tương ứng>` hoặc sử dụng phím tắt `shift` + `-` --> chuột phải tại mã hash đó --> click `HashDB Lookup` rồi chú ý bảng Output hiển thị tên hàm được query.

    ![alt text](images/18.png)
# THUẬT TOÁN RC4

## **[1]** Tổng quan
RC4 là một thuật toán mã hóa dòng tạo ra một chuỗi bit giả ngẫu nhiên (keystream). Việc mã hóa và giải mã đều được thực hiện bằng cách XOR chuỗi keystream này với bản rõ hoặc bản mã.

Thuật toán RC4 có hai giai đoạn chính:
- **Key-Scheduling Algorithm (KSA):**<br>
Thuật toán KSA được dùng để khởi tạo mảng hoán vị S. Độ dài khóa (keylength) là số byte trong khóa, có thể từ 1 đến 256 (thường là 5 đến 16 byte, tương đương 40–128 bit). Đầu tiên, mảng S được khởi tạo theo hoán vị đơn vị. Sau đó, S được trộn qua 256 vòng lặp, đồng thời kết hợp với từng byte của khóa.
    ```
    for i từ 0 đến 255:
        S[i] := i
    j := 0
    for i từ 0 đến 255:
        j := (j + S[i] + key[i mod keylength]) mod 256
        hoán đổi S[i] và S[j]
    ```
- **Pseudo-Random Generation Algorithm (PRGA):**<br>
Thuật toán PRGA tạo keystream bằng cách thay đổi trạng thái S và xuất ra từng byte một. Mỗi vòng lặp:
    - Tăng i
    - Cộng giá trị S[i] vào j
    - Hoán đổi S[i] và S[j]
    - Tính chỉ số t = (S[i] + S[j]) mod 256
    - Giá trị keystream K = S[t]
    ```
    i := 0
    j := 0
    trong khi cần sinh output:
        i := (i + 1) mod 256
        j := (j + S[i]) mod 256
        hoán đổi S[i] và S[j]
        t := (S[i] + S[j]) mod 256
        K := S[t]
        xuất K
    ```
    - Cuối cùng chuỗi K[0], K[1], ... được XOR với bản rõ để tạo ra bản mã: `ciphertext[i] = plaintext[i] ^ K[i]`

## **[2]** Code mô phỏng bằng MASM
- **Source:** [RC4 Algorithm](RC4\rc4.asm)
- **Code:**
```asm
.386
.model flat, stdcall
option casemap: none

; Dinh nghia gia tri truyen vao cac ham WinAPI
STD_INPUT_HANDLE  equ -10
STD_OUTPUT_HANDLE equ -11
NULL              equ 0

; Dinh nghia nguyen mau cac ham WinAPI
ExitProcess      PROTO STDCALL :DWORD
GetStdHandle     PROTO STDCALL :DWORD
ReadConsoleA     PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
WriteConsoleA    PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD

.data
    ; Khai bao cac bien
    szInputPrompt   db "Plaintext: ", 0
    szInputLen      dd $ - szInputPrompt
    szKeyPrompt     db "Key: ", 0
    szKeyLen        dd $ - szKeyPrompt
    szOutputPrompt  db "Ciphertext (HEX): ", 0
    szOutputLen     dd $ - szOutputPrompt
    crlf            db 13, 10
    
    plaintext       db 256 dup(0)
    key             db 256 dup(0)
    ciphertext      db 256 dup(0)
    hexOutput       db 768 dup(0)   ; hex chua 2 ky tu/byte, moi byte cach nhau boi dau cach
    
    ; Cac bien cho RC4
    S               db 256 dup(0)   ; mang S de khoi tao S-box
    plaintextLen    dd 0            ; lay do dai plaintext
    keyLen          dd 0            ; lay do dai key

.data?
    hStdIn          dd ?            ; handle toi stdin, dung voi WinAPI ReadConsole
    hStdOut         dd ?            ; handle toi stdout, dung voi WinAPI WriteConsoleA
    bytesRead       dd ?            ; luu so byte thuc su doc duoc sau khi goi ReadConsole
    bytesWritten    dd ?            ; luu so byte thuc su ghi ra sau khi goi WriteConsoleA
    
.code
start:
    ; lay handle cua stdin
    push STD_INPUT_HANDLE
    call GetStdHandle               ; Ham GetStdHandle nhan vao 1 tham so duy nhat STD_[INPUT/OUTPUT/ERROR]_HANDLE
    mov hStdIn, eax
    
    ; Lấy handle của stdout
    push STD_OUTPUT_HANDLE
    call GetStdHandle
    mov hStdOut, eax
    
    ; Hien thi prompt "Plaintext: "
    push NULL                       ; lpReserved - nhan gia tri NULL duy nhat
    push offset bytesWritten        ; lpNumberOfCharsWritten - [out]
                                    ; sau khi goi ham se tra ve tai bien nay
    push [szInputLen]               ; nNumberOfCharsToWrite - [in]
                                    ; can phai khoi tao ngay khi khai bao
    push offset szInputPrompt       ; lpBuffer - [in]
                                    ; duoc khai bao va khoi tao tai section .data
    push [hStdOut]                  ; hConsoleOutput - [in]
                                    ; handle toi stdout
    call WriteConsoleA              ; WriteConsoleA(hConsoleOutput,*lpBuffer,nNumberOfCharsToWrite,lpNumberOfCharsWritten,lpReserved)
                                    ; khi goi ham WriteConsoleA, chuong trinh lay lan luot tu dinh stack
    
    ; Doc input tu ban phim
    push NULL                       ; pInputControl - [in] khong bat buoc nhan gia tri cu the
    push offset bytesRead           ; lpNumberOfCharsRead - [out]
                                    ; lay dia chi cua byte NULL sau khi nhan input
    push 255                        ; nNumberOfCharsToRead - [in]
                                    ; la so ky tu toi da duoc phep doc
    push offset plaintext           ; lpBuffer - [out]
                                    ; bien nay duoc gan sau khi da nhan input tu nguoi dung
    push [hStdIn]                   ; hConsoleInput - [in], handle toi stdin
    call ReadConsoleA               ; ReadConsoleA(hConsoleInput, lpBuffer, nNumberOfCharsToRead, lpNumberOfCharsRead, pInputControl)
                                    ; khi goi ham ReadConsoleA, chuong trinh se lay lan luot tu dinh stack
    
    ; Tinh do dai thuc su cua input
    mov eax, [bytesRead]            ; gia tri tai dia chi bytesRead lay tu ham ReadConsoleA
    sub eax, 2                      ; tru byte '\n' va byte NULL
    mov [plaintextLen], eax         ; gan gia tri vao vung nho cua bien plaintextLen
    
    ; Hien thi prompt "Key: "
    push NULL
    push offset bytesWritten
    push [szKeyLen]
    push offset szKeyPrompt
    push [hStdOut]
    call WriteConsoleA
    
    ; Doc key tu ban phim
    push NULL
    push offset bytesRead
    push 255
    push offset key
    push [hStdIn]
    call ReadConsoleA
    
    ; Lay do dai cua key
    mov eax, [bytesRead]
    sub eax, 2
    mov [keyLen], eax
    
    ; Ma hoa RC4
    call RC4_KSA        ; Key Scheduling Algorithm
    call RC4_PRGA       ; Pseudo-Random Generation Algorithm
    
    ; Chuyen ket qua sang Hex
    call ConvertToHex

    push eax            ; eax dang luu gia tri length cua hexOutput
                        ; push vao stack de khong bi ghi de sau khi goi WriteConsoleA
    
    ; Hien thi prompt "Ciphertext (HEX): "
    push NULL
    push offset bytesWritten
    push [szOutputLen]
    push offset szOutputPrompt
    push [hStdOut]
    call WriteConsoleA

    pop eax             ; lay eax tu stack lam do dai hexOutput
    
    ; Hien thi cipher duoi dang hex
    push NULL
    push offset bytesWritten
    push eax
    push offset hexOutput
    push [hStdOut]
    call WriteConsoleA
    
    ; Hien thi ky tu '\n'
    push NULL
    push offset bytesWritten
    push 2
    push offset crlf
    push [hStdOut]
    call WriteConsoleA
    
    ; Thoát chương trình
    push 0
    call ExitProcess

; RC4 Key Scheduling Algorithm (KSA)
RC4_KSA PROC
    ; Khoi tao S-box
    xor ecx, ecx                ; bien lap cua mang S
init_loop:
    mov byte ptr [S + ecx], cl  ; S[i] = i
    inc ecx                     ; i++
    cmp ecx, 256                ; i < 256 -> loop
    jnz init_loop
    
    ; Hoan vi S-box dua tren key
    xor eax, eax    ; i = 0
    xor ebx, ebx    ; j = 0
    
ksa_loop:
    mov ecx, eax
    
    add bl, byte ptr [S + ecx]          ; j += S[i]
    
    mov eax, ecx                        ; Khoi phuc eax = i
    xor edx, edx                        ; Xoa edx de chuan bi phep chia
    div dword ptr [keyLen]              ; edx = i % keyLen, eax = i / keyLen
    
    add bl, byte ptr [key + edx]        ; j += key[i mod keyLen]
    and ebx, 0FFh                       ; j = j mod 256
    
    ; Swap(S[i], S[j])
    mov al, byte ptr [S + ecx]          ; al = S[i]
    mov dl, byte ptr [S + ebx]          ; dl = S[j]
    mov byte ptr [S + ecx], dl          ; S[i] = dl
    mov byte ptr [S + ebx], al          ; S[j] = al
    
    ; i = (i + 1) mod 256
    mov eax, ecx                        ; Khoi phuc eax = i
    inc eax                             ; i++
    cmp eax, 256
    jnz ksa_loop
    
    ret
RC4_KSA ENDP

; RC4 Pseudo-Random Generation Algorithm (PRGA)
RC4_PRGA PROC
    xor eax, eax                        ; i = 0 ~ Lap S
    xor ebx, ebx                        ; j = 0
    xor ecx, ecx                        ; counter ~ lap plaintext
    
prga_loop:
    ; i = (i + 1) mod 256
    inc eax
    and eax, 0FFh
    
    ; j = (j + S[i]) mod 256
    mov dl, byte ptr [S + eax]          ; dl = S[i]
    add bl, dl                          ; j += S[i]
    and ebx, 0FFh                       ; j %= 256
    
    ; Swap (S[i], S[j])
    mov dl, byte ptr [S + eax]          ; dl = S[i]
    mov dh, byte ptr [S + ebx]          ; dh = S[j]
    mov byte ptr [S + eax], dh          ; S[i] = dh
    mov byte ptr [S + ebx], dl          ; S[j] = dl
    
    ; K = S[(S[i] + S[j]) mod 256]
    mov dl, byte ptr [S + eax]          ; dl = S[i]
    add dl, byte ptr [S + ebx]          ; dl += S[j]
    and edx, 0FFh                       ; edx %= 256
    mov dl, byte ptr [S + edx]          ; dl = S[edx]
    
    ; ciphertext[counter] = plaintext[counter] XOR K
    xor dl, byte ptr [plaintext + ecx]
    mov byte ptr [ciphertext + ecx], dl
    
    inc ecx                             ; counter++
    cmp ecx, [plaintextLen]             ; counter < strlen(plaintext)
    jnz prga_loop
    
    ret
RC4_PRGA ENDP

; Chuyen cac byte sang HEX
ConvertToHex PROC
    xor ecx, ecx                        ; i ~ lap input
    xor edx, edx                        ; j ~ lap output
    
hex_loop:
    ; Kiem tra xem da xu ly het cac byte trong ciphertext chua
    cmp ecx, [plaintextLen]
    jge hex_done
    
    ; Lay byte can chuyen doi
    movzx eax, byte ptr [ciphertext + ecx]
    
    ; High nibble (4 bit cao)
    mov ebx, eax
    shr ebx, 4                          ; Dich phai 4 bit de lay 4 bit cao
    cmp ebx, 10
    jl high_digit
    add ebx, 'A' - '0' - 10             ; Neu >= 10, chuyen thanh 'A'-'F'
high_digit:
    add ebx, '0'                        ; Chuyen thanh ky tu ASCII
    mov byte ptr [hexOutput + edx], bl
    inc edx                             ; j++
    
    ; Low nibble (4 bit thap)
    and eax, 0Fh                        ; Lay 4 bit thap
    cmp eax, 10
    jl low_digit
    add eax, 'A' - '0' - 10             ; Neu >= 10, chuyen thanh 'A'-'F'
low_digit:
    add eax, '0'                        ; Chuyen thanh ky tu ASCII
    mov byte ptr [hexOutput + edx], al
    inc edx                             ; j++
    
    ; Them dau cach giua cac byte
    mov byte ptr [hexOutput + edx], ' '
    inc edx                             ; j++
    
    inc ecx                             ; i++
    jmp hex_loop
    
hex_done:
    dec edx                             ; Xoa dau cach cuoi cung
    
    mov byte ptr [hexOutput + edx], 0   ; Them ky tu ket thuc chuoi
    
    mov eax, edx                        ; Tra ve do dai chuoi hex
    ret
ConvertToHex ENDP

end start
```

## **[3]** Tài liệu tham khảo
- *Thuật toán:* [Wiki - RC4](https://en.wikipedia.org/wiki/RC4)
- *Code masm:* [masm - YouTube](https://www.youtube.com/playlist?list=PLan2CeTAw3pFOq5qc9urw8w7R-kvAT8Yb)
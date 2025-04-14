.386
.model flat, stdcall
option casemap:none

; Khai báo các hằng số
STD_OUTPUT      EQU -11

.data
    msg             db 'Hello World', 0Dh, 0Ah, 0
    MsgLen          dd $ - msg - 1
    BytesWritten    dd 0
    hConsole        dd 0
    hKernel32       dd 0

    ; Biến lưu địa chỉ export table
    AddressOfNamesVA      dd 0
    AddressOfNameOrdsVA   dd 0
    AddressOfFunctionsVA  dd 0

    ; Tên các DLL và hàm cần sử dụng (thêm terminating zeros cho an toàn)
    szKernel32           db 'kernel32.dll', 0
    szGetProcAddress     db 'GetProcAddress', 0
    szLoadLibraryA       db 'LoadLibraryA', 0
    szWriteConsoleA      db 'WriteConsoleA', 0
    szGetStdHandle       db 'GetStdHandle', 0
    szExitProcess        db 'ExitProcess', 0

    ; Địa chỉ các hàm sẽ được resolve
    _GetProcAddress      dd 0
    _GetStdHandle        dd 0
    _WriteConsoleA       dd 0
    _ExitProcess         dd 0

.code
start:
    ; Bước 1: Lấy kernel32.dll base address (đã có trong PEB)
    assume fs:nothing
    
    mov eax, fs:[30h]            ; PEB (Process Environment Block)
    mov eax, [eax+0Ch]           ; Ldr
    mov eax, [eax+14h]           ; InMemoryOrderModuleList
    mov eax, [eax]               ; next entry (ntdll.dll)
    mov eax, [eax]               ; next entry (kernel32.dll)
    mov eax, [eax+10h]           ; BaseAddress của kernel32.dll
    mov [hKernel32], eax         ; Lưu base address của kernel32.dll
    
    ; Bước 2: Tìm GetProcAddress và các hàm khác bằng Export Table
    
    ; Lấy địa chỉ của PE header
    mov ebx, eax                 ; ebx = kernel32 base address
    mov eax, [ebx+3Ch]           ; e_lfanew - RVA của PE header
    add eax, ebx                 ; eax = địa chỉ thực của PE header
    
    ; Lấy data directory RVA
    mov eax, [eax+78h]           ; Export Directory RVA
    add eax, ebx                 ; eax = Export Directory VA
    
    ; Lấy thông tin từ export directory
    mov ecx, [eax+18h]           ; NumberOfNames
    mov edx, [eax+20h]           ; AddressOfNames RVA
    add edx, ebx                 ; edx = AddressOfNames VA
    mov [AddressOfNamesVA], edx  ; Lưu lại để sử dụng sau
    
    ; Lưu AddressOfNameOrdinals và AddressOfFunctions
    mov esi, [eax+24h]           ; AddressOfNameOrdinals RVA
    add esi, ebx                 ; esi = AddressOfNameOrdinals VA
    mov [AddressOfNameOrdsVA], esi ; Lưu lại để sử dụng sau
    
    mov edi, [eax+1Ch]           ; AddressOfFunctions RVA
    add edi, ebx                 ; edi = AddressOfFunctions VA
    mov [AddressOfFunctionsVA], edi ; Lưu lại để sử dụng sau
    
    ; Duyệt qua các export names để tìm GetProcAddress
    xor eax, eax                 ; eax = index = 0
    
search_loop:
    push eax                     ; Lưu index hiện tại
    push ecx                     ; Lưu số lượng tên hàm còn lại
    
    mov ecx, [edx + eax*4]       ; Lấy RVA của tên hàm
    test ecx, ecx
    jz next_name
    
    add ecx, ebx                 ; ecx = VA của tên hàm
    
    ; So sánh với "GetProcAddress"
    push ebx                     ; Lưu giá trị của ebx
    mov esi, offset szGetProcAddress
    
compare_name:
    mov bl, byte ptr [ecx]       ; Lấy ký tự từ tên export
    mov bh, byte ptr [esi]       ; Lấy ký tự từ "GetProcAddress"
    
    test bl, bl                  ; Kiểm tra ký tự export có phải null không
    jz check_end_of_target
    
    test bh, bh                  ; Kiểm tra ký tự target có phải null không
    jz name_not_match
    
    cmp bl, bh                   ; So sánh 2 ký tự
    jne name_not_match           ; Nếu khác nhau, không phải hàm cần tìm
    
    inc ecx                      ; Tăng con trỏ tên export
    inc esi                      ; Tăng con trỏ tên target
    jmp compare_name             ; Tiếp tục so sánh
    
check_end_of_target:
    cmp byte ptr [esi], 0        ; Kiểm tra xem cả 2 chuỗi đều kết thúc chưa
    jne name_not_match           ; Nếu target chưa kết thúc, không phải hàm cần tìm
    
    ; Đã tìm thấy GetProcAddress
    pop ebx                      ; Khôi phục ebx = kernel32 base address
    pop ecx                      ; Khôi phục số lượng tên hàm
    pop eax                      ; Khôi phục index
    
    mov esi, [AddressOfNameOrdsVA]      ; Lấy AddressOfNameOrdinals VA
    
    ; Lấy ordinal
    movzx ecx, word ptr [esi + eax*2]   ; Lấy ordinal tương ứng
    
    ; Lấy địa chỉ hàm thông qua ordinal
    mov edi, [AddressOfFunctionsVA]     ; Lấy AddressOfFunctions VA
    mov eax, [edi + ecx*4]       ; Lấy RVA của hàm GetProcAddress
    add eax, [hKernel32]         ; eax = VA của GetProcAddress
    
    mov [_GetProcAddress], eax   ; Lưu địa chỉ của GetProcAddress
    
    jmp got_getprocaddress       ; Chuyển đến phần tiếp theo
    
name_not_match:
    pop ebx                      ; Khôi phục ebx
    
next_name:
    pop ecx                      ; Khôi phục số lượng tên hàm
    pop eax                      ; Khôi phục index
    
    inc eax                      ; Tăng index
    dec ecx                      ; Giảm số lượng tên hàm còn lại
    jnz search_loop              ; Tiếp tục tìm nếu chưa hết tên
    
got_getprocaddress:
    ; Sử dụng GetProcAddress để lấy các hàm khác
    push offset szGetStdHandle      ; Tên hàm hoặc biến cần resolve
    push [hKernel32]                ; Handle tới kernel32.dll (dll chứa hàm hoặc biến cần resolve)
    call [_GetProcAddress]          ; GetProcAddress(hModule, lpProcName) --> return địa chỉ của hàm cần gọi ra thanh ghi eax
    mov [_GetStdHandle], eax
    
    push offset szWriteConsoleA
    push [hKernel32]
    call [_GetProcAddress]
    mov [_WriteConsoleA], eax
    
    push offset szExitProcess
    push [hKernel32]
    call [_GetProcAddress]
    mov [_ExitProcess], eax
    
    ; Bước 3: Sử dụng GetStdHandle để lấy handle của stdout
    push STD_OUTPUT              ; STD_OUTPUT_HANDLE = -11
    call [_GetStdHandle]
    mov [hConsole], eax
    
    ; Bước 4: Sử dụng WriteConsoleA để hiển thị "Hello World"
    push offset BytesWritten     ; lpNumberOfCharsWritten
    push [MsgLen]                ; nNumberOfCharsToWrite
    push offset msg              ; lpBuffer
    push [hConsole]              ; hConsoleOutput
    call [_WriteConsoleA]
    
    ; Kết thúc chương trình
    push 0                       ; EXIT_SUCCESS
    call [_ExitProcess]
        
    ; Thoát chương trình với mã lỗi
    mov eax, 1
    ret

end start
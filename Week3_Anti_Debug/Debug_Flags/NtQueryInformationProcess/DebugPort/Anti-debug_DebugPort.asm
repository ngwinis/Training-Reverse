.386
.model flat, stdcall
option casemap:none

; Thủ công khai báo các hàm Windows API
ExitProcess proto stdcall :DWORD
MessageBoxA proto stdcall :DWORD, :DWORD, :DWORD, :DWORD
GetProcAddress proto stdcall :DWORD, :DWORD
LoadLibraryA proto stdcall :DWORD
GetCurrentProcess proto stdcall

.data
szNtdll db "ntdll.dll", 0
szNtQueryInformationProcess db "NtQueryInformationProcess", 0
szDetected db "Debugger Detected!", 0
szNotDetected db "No Debugger Detected", 0
szCaption db "Anti-Debug", 0

NtQueryInformationProcess dd 0
dwProcessDebugPort dd 0
dwReturned dd 0

.code
start:
    ; LoadLibraryA("ntdll.dll")
    push offset szNtdll
    call LoadLibraryA
    test eax, eax
    jz fail
    mov ebx, eax ; lưu hModule ntdll

    ; GetProcAddress(hNtdll, "NtQueryInformationProcess")
    push offset szNtQueryInformationProcess
    push ebx
    call GetProcAddress
    test eax, eax
    jz fail
    mov [NtQueryInformationProcess], eax

    ; Gọi NtQueryInformationProcess
    lea eax, [dwReturned]
    push eax
    push 4
    lea eax, [dwProcessDebugPort]
    push eax
    push 7 ; ProcessDebugPort
    call GetCurrentProcess
    push eax
    call [NtQueryInformationProcess]

    ; So sánh với -1
    cmp [dwProcessDebugPort], -1
    je detected

    ; Không phát hiện debugger
    push 0
    push offset szCaption
    push offset szNotDetected
    push 0
    call MessageBoxA
    jmp exit

detected:
    push 0
    push offset szCaption
    push offset szDetected
    push 0
    call MessageBoxA

exit:
    push 0
    call ExitProcess

fail:
    push 0
    call ExitProcess

end start

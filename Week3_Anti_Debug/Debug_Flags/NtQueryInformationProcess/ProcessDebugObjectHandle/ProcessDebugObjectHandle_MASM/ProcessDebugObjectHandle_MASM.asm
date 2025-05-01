.386
.model flat, stdcall
option casemap:none

ExitProcess proto stdcall :DWORD
MessageBoxA proto stdcall :DWORD, :DWORD, :DWORD, :DWORD
LoadLibraryA proto stdcall :DWORD
GetProcAddress proto stdcall :DWORD, :DWORD
GetCurrentProcess proto stdcall

.data
szNtdll db "ntdll.dll", 0
szNtQueryInformationProcess db "NtQueryInformationProcess", 0
szDebuggerDetected db "Debugger Detected!", 0
szNoDebuggerDetected db "No Debugger Detected", 0
szCaption db "Anti-Debug", 0

NtQueryInformationProcess dd 0
dwReturned dd 0
dwProcessDebugObject dd 0

ProcessDebugObjectHandle equ 1Eh

.code
start:
    push offset szNtdll
    call LoadLibraryA
    test eax, eax
    jz exit
    mov ebx, eax

    push offset szNtQueryInformationProcess
    push ebx
    call GetProcAddress
    test eax, eax
    jz exit
    mov [NtQueryInformationProcess], eax

    lea eax, [dwReturned]
    push eax
    push 4
    lea eax, [dwProcessDebugObject]
    push eax
    push ProcessDebugObjectHandle
    call GetCurrentProcess
    push eax
    call [NtQueryInformationProcess]
    test eax, eax

    cmp [dwProcessDebugObject], 0
    je no_debugger

    push 0
    push offset szCaption
    push offset szDebuggerDetected
    push 0
    call MessageBoxA
    jmp exit

no_debugger:
    push 0
    push offset szCaption
    push offset szNoDebuggerDetected
    push 0
    call MessageBoxA

exit:
    push 0
    call ExitProcess


end start

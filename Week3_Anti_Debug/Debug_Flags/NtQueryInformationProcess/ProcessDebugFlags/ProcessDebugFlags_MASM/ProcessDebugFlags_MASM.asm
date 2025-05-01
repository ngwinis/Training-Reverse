.386
.model flat, stdcall
option casemap:none

; Function prototypes
LoadLibraryA PROTO :DWORD
GetProcAddress PROTO :DWORD, :DWORD
ExitProcess PROTO :DWORD
FreeLibrary PROTO :DWORD
MessageBoxA PROTO :DWORD, :DWORD, :DWORD, :DWORD

; Constants
ProcessDebugFlags equ 1Fh
NULL equ 0
MB_OK equ 0
MB_ICONERROR equ 10h

.data
szAlertCaption  db "Alert", 0
szOkCaption     db "OK", 0
szDebuggerMsg   db "Debugger Detected!", 0
szNoDebuggerMsg db "No Debugger", 0
hNtdll          dd 0
dwProcessDebugFlags dd 0
dwReturned      dd 0
szNtdll         db "ntdll.dll", 0
szNtQueryInformationProcess db "NtQueryInformationProcess", 0

.code
start:
    ; Attempt to detect debugger
    call IsDebuggerPresent_DebugFlags
    
    ; Check result in EAX (1 = debugger detected, 0 = no debugger)
    test eax, eax
    jz no_debugger
    
    ; Debugger detected
    push MB_OK or MB_ICONERROR
    push offset szAlertCaption
    push offset szDebuggerMsg
    push NULL
    call MessageBoxA
    
    push 1          ; Return code 1 (error)
    call ExitProcess
    
no_debugger:
    ; No debugger detected
    push MB_OK
    push offset szOkCaption
    push offset szNoDebuggerMsg
    push NULL
    call MessageBoxA
    
    push 0          ; Return code 0 (success)
    call ExitProcess

; Function to detect debugger using ProcessDebugFlags
; Returns: EAX = 1 if debugger detected, 0 if not detected
IsDebuggerPresent_DebugFlags proc
    ; Load ntdll.dll
    push offset szNtdll
    call LoadLibraryA
    test eax, eax
    jz func_fail
    
    mov hNtdll, eax
    
    ; Get address of NtQueryInformationProcess
    push offset szNtQueryInformationProcess
    push hNtdll
    call GetProcAddress
    test eax, eax
    jz cleanup_fail
    
    ; Store function pointer in EDI
    mov edi, eax
    
    ; Call NtQueryInformationProcess
    lea edx, [dwReturned]
    push edx                    ; ReturnLength
    push 4                      ; ProcessInformationLength (sizeof(DWORD))
    lea edx, [dwProcessDebugFlags]
    push edx                    ; ProcessInformation
    push ProcessDebugFlags      ; ProcessInformationClass (0x1F)
    push -1                     ; ProcessHandle (GetCurrentProcess = -1)
    call edi                    ; Call NtQueryInformationProcess
    
    ; Check return status in EAX (0 = success)
    test eax, eax
    jnz cleanup_fail
    
    ; Check if debugger is present
    ; dwProcessDebugFlags == 0 means debugger is present
    cmp dword ptr [dwProcessDebugFlags], 0
    jne cleanup_fail
    
    ; Debugger detected
    push hNtdll
    call FreeLibrary
    mov eax, 1                  ; Return 1 (debugger detected)
    ret
    
cleanup_fail:
    ; No debugger or failed check
    push hNtdll
    call FreeLibrary
    
func_fail:
    xor eax, eax               ; Return 0 (no debugger detected)
    ret
IsDebuggerPresent_DebugFlags endp

end start
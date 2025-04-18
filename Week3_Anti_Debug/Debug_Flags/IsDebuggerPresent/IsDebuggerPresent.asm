.386
.model	flat, stdcall
option	casemap:none

ExitProcess			PROTO STDCALL :DWORD
GetStdHandle		PROTO STDCALL :DWORD
WriteConsoleA		PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
IsDebuggerPresent	PROTO STDCALL

.data
	szDbgDetectManual		db "Debugger detected (manual).", 0
	szDbgDetectManualLen	dd $ - szDbgDetectManual
	szNoDbgManual			db "No Debugger (manual).", 0
	szNoDbgManualLen		dd $ - szNoDbgManual
	szDbgDetectAPI			db "Debugger detected (API)", 0
	szDbgDetectAPILen		dd $ - szDbgDetectAPI
	szNoDbgAPI				db "No Debugger (API).", 0
	szNoDbgAPILen			dd $ - szNoDbgAPI
	crlf					db 13, 10

.data?
	hStdOut					dd ?
	bytesWritten			dd ?

.code
start:
	push	-11
	call	GetStdHandle
	mov		hStdOut, eax
	
	; Kiểm tra thủ công
	assume	fs:nothing

	mov		eax, fs:[30h]
	cmp		byte ptr [eax+2], 0
	jne		being_debugged_manual
	lea		ebx, szNoDbgManual
	mov		edx, szNoDbgManualLen
	call	print

being_debugged_manual:
	lea		ebx, szDbgDetectManual
	mov		edx, szDbgDetectManualLen
	call	print

	; Xuống dòng
	lea		ebx, crlf
	mov		edx, 2
	call	print

	; Kiểm tra bằng Windows API
	call	IsDebuggerPresent
	cmp		eax, 0
	jne		being_debugged_API
	lea		ebx, szNoDbgAPI
	mov		edx, szNoDbgAPILen
	call	print

being_debugged_API:
	lea		ebx, szDbgDetectAPI
	mov		edx, szDbgDetectAPILen
	call	print

	lea		ebx, crlf
	mov		edx, 2
	call	print
	
print PROC
	push	0
	push	offset bytesWritten
	push	edx
	push	ebx
	push	[hStdOut]
	call	WriteConsoleA

	ret
print endp

	push	0
	call	ExitProcess

end start
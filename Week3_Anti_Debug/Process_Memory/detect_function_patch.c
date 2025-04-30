#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

BOOL IsDebuggerPatched()
{
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32)
        return FALSE;

    FARPROC pIsDebuggerPresent = GetProcAddress(hKernel32, "IsDebuggerPresent");
    if (!pIsDebuggerPresent)
        return FALSE;

    DWORD dwOriginalBytes = 0;
    memcpy(&dwOriginalBytes, pIsDebuggerPresent, sizeof(DWORD));

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return FALSE;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);
    if (!Process32FirstW(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return FALSE;
    }

    BOOL bDetected = FALSE;
    const DWORD dwCurrentPID = GetCurrentProcessId();
    HANDLE hProcess = NULL;
    DWORD dwRemoteBytes = 0;

    do {
        if (pe.th32ProcessID == dwCurrentPID)
            continue;

        hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
        if (!hProcess)
            continue;

        if (ReadProcessMemory(hProcess, pIsDebuggerPresent, &dwRemoteBytes, sizeof(DWORD), NULL)) {
            if (dwRemoteBytes != dwOriginalBytes) {
                bDetected = TRUE;
                CloseHandle(hProcess);
                break;
            }
        }
        CloseHandle(hProcess);
    } while (Process32NextW(hSnapshot, &pe));

    CloseHandle(hSnapshot);
    return bDetected;
}

int main()
{
    if (IsDebuggerPatched()) {
        MessageBoxA(NULL, "Debugger patch detected!", "Anti-Debug", MB_OK | MB_ICONWARNING);
    } else {
        MessageBoxA(NULL, "No debugger interference.", "Anti-Debug", MB_OK | MB_ICONINFORMATION);
    }
    return 0;
}

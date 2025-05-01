#include <windows.h>
#include <winternl.h>

typedef NTSTATUS(NTAPI* TNtQueryInformationProcess)(
    HANDLE ProcessHandle,
    DWORD ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );

bool IsDebuggerPresent_DebugFlags()
{
    HMODULE hNtdll = LoadLibraryA("ntdll.dll");
    if (!hNtdll) return false;

    auto pfnNtQueryInformationProcess = (TNtQueryInformationProcess)GetProcAddress(
        hNtdll, "NtQueryInformationProcess");
    if (!pfnNtQueryInformationProcess) return false;

    DWORD dwProcessDebugFlags = 0;
    DWORD dwReturned = 0;
    const DWORD ProcessDebugFlags = 0x1F;

    NTSTATUS status = pfnNtQueryInformationProcess(
        GetCurrentProcess(),
        ProcessDebugFlags,
        &dwProcessDebugFlags,
        sizeof(DWORD),
        &dwReturned
    );

    return (status == 0 && dwProcessDebugFlags == 0);
}

int main()
{
    if (IsDebuggerPresent_DebugFlags())
    {
        MessageBoxA(NULL, "Debugger Detected!", "Alert", MB_OK | MB_ICONERROR);
        return 1;
    }

    MessageBoxA(NULL, "No Debugger", "OK", MB_OK);
    return 0;
}

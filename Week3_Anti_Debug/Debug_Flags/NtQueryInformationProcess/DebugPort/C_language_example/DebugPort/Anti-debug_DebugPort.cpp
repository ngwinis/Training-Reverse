#include <Windows.h>
#include <winternl.h>
#include <stdbool.h>

typedef NTSTATUS(NTAPI* TNtQueryInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );

bool IsDebuggerPresent_DebugPort()
{
    HMODULE hNtdll = LoadLibraryA("ntdll.dll");
    if (!hNtdll) return false;

    auto pfnNtQueryInformationProcess = (TNtQueryInformationProcess)GetProcAddress(
        hNtdll, "NtQueryInformationProcess");

    if (!pfnNtQueryInformationProcess) return false;

    DWORD dwProcessDebugPort = 0;
    DWORD dwReturned = 0;
    NTSTATUS status = pfnNtQueryInformationProcess(
        GetCurrentProcess(),
        ProcessDebugPort,
        &dwProcessDebugPort,
        sizeof(DWORD),
        &dwReturned
    );

    return (status == 0 && dwProcessDebugPort == (DWORD)-1);
}

int main()
{
    if (IsDebuggerPresent_DebugPort())
    {
        MessageBoxA(NULL, "Debugger Detected!", "Warning", MB_OK | MB_ICONERROR);
        return 1;
    }

    MessageBoxA(NULL, "No Debugger", "Info", MB_OK | MB_ICONINFORMATION);
    return 0;
}

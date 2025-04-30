#include <Windows.h>
#include <iostream>

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation = 0
} PROCESSINFOCLASS;

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PVOID PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

typedef LONG(NTAPI* pNtQueryInformationProcess)(
    HANDLE,
    PROCESSINFOCLASS,
    PVOID,
    ULONG,
    PULONG
    );

bool IsDebugged()
{
    HWND hExplorerWnd = GetShellWindow();
    if (!hExplorerWnd)
        return false;

    DWORD dwExplorerProcessId = 0;
    GetWindowThreadProcessId(hExplorerWnd, &dwExplorerProcessId);

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll)
        return false;

    pNtQueryInformationProcess NtQueryInformationProcess =
        (pNtQueryInformationProcess)GetProcAddress(hNtdll, "NtQueryInformationProcess");
    if (!NtQueryInformationProcess)
        return false;

    PROCESS_BASIC_INFORMATION pbi = { 0 };
    LONG status = NtQueryInformationProcess(
        GetCurrentProcess(),
        ProcessBasicInformation,
        &pbi,
        sizeof(pbi),
        NULL
    );

    if (status != 0)
        return false;

    return ((DWORD)pbi.InheritedFromUniqueProcessId != dwExplorerProcessId);
}

int main()
{
    if (IsDebugged())
        std::cout << "Debugger detected!\n";
    else
        std::cout << "No debugger.\n";
    std::cout << "\nPress any key to terminate!";
    getchar();

    return 0;
}

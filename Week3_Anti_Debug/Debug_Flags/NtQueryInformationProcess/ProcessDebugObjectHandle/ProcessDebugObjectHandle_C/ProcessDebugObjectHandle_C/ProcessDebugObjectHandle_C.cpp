#include <Windows.h>
#include <stdio.h>

#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

typedef NTSTATUS(NTAPI* TNtQueryInformationProcess)(
    IN HANDLE           ProcessHandle,
    IN DWORD            ProcessInformationClass,
    OUT PVOID           ProcessInformation,
    IN ULONG            ProcessInformationLength,
    OUT PULONG          ReturnLength
    );

BOOL IsBeingDebugged()
{
    HMODULE hNtdll = LoadLibraryA("ntdll.dll");
    if (!hNtdll)
    {
        printf("Failed to load ntdll.dll\n");
        return FALSE;
    }

    auto pfnNtQueryInformationProcess = (TNtQueryInformationProcess)GetProcAddress(
        hNtdll, "NtQueryInformationProcess");

    if (!pfnNtQueryInformationProcess)
    {
        printf("Failed to get NtQueryInformationProcess address\n");
        FreeLibrary(hNtdll);
        return FALSE;
    }

    DWORD dwReturned;
    HANDLE hProcessDebugObject = 0;
    const DWORD ProcessDebugObjectHandle = 0x1e;

    NTSTATUS status = pfnNtQueryInformationProcess(
        GetCurrentProcess(),
        ProcessDebugObjectHandle,
        &hProcessDebugObject,
        sizeof(HANDLE),
        &dwReturned);

    FreeLibrary(hNtdll);
    if (NT_SUCCESS(status) && (0 != hProcessDebugObject))
    {
        return TRUE;
    }

    return FALSE;
}

int main()
{
    printf("Checking for debugger...\n");

    if (IsBeingDebugged())
    {
        printf("Debugger detected!\n");
    }
    else {
        printf("No debugger detected!\n");
    }

    return 0;
}
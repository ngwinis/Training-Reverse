#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>

DWORD GetParentProcessId(DWORD dwCurrentProcessId)
{
    DWORD dwParentProcessId = -1;
    PROCESSENTRY32W ProcessEntry = { 0 };
    ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return -1;

    if (Process32FirstW(hSnapshot, &ProcessEntry))
    {
        do
        {
            if (ProcessEntry.th32ProcessID == dwCurrentProcessId)
            {
                dwParentProcessId = ProcessEntry.th32ParentProcessID;
                break;
            }
        } while (Process32NextW(hSnapshot, &ProcessEntry));
    }

    CloseHandle(hSnapshot);
    return dwParentProcessId;
}

bool IsDebugged()
{
    bool bDebugged = false;
    DWORD dwParentProcessId = GetParentProcessId(GetCurrentProcessId());

    if (dwParentProcessId == (DWORD)-1)
        return false;

    PROCESSENTRY32W ProcessEntry = { 0 };
    ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return false;

    if (Process32FirstW(hSnapshot, &ProcessEntry))
    {
        do
        {
            if (ProcessEntry.th32ProcessID == dwParentProcessId)
            {
                if (_wcsicmp(ProcessEntry.szExeFile, L"explorer.exe") != 0)
                {
                    bDebugged = true;
                }
                break;
            }
        } while (Process32NextW(hSnapshot, &ProcessEntry));
    }

    CloseHandle(hSnapshot);
    return bDebugged;
}

int wmain()
{
    if (IsDebugged())
        wprintf(L"[!] Debugger detected (parent is NOT explorer.exe)\n");
    else
        wprintf(L"[+] No debugger detected (parent is explorer.exe)\n");
    getchar();

    return 0;
}

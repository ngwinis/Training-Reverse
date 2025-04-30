#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#define EVENT_SELFDBG_EVENT_NAME L"SelfDebugging"

bool EnableDebugPrivilege()
{
    bool bResult = false;
    HANDLE hToken = NULL;

    do
    {
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
            break;

        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
            break;

        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
            break;

        bResult = true;
    } while (0);

    if (hToken)
        CloseHandle(hToken);

    return bResult;
}

bool IsDebugged()
{
    WCHAR wszFilePath[MAX_PATH] = { 0 }, wszCmdLine[MAX_PATH] = { 0 };
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    HANDLE hDbgEvent;

    hDbgEvent = CreateEventW(NULL, FALSE, FALSE, EVENT_SELFDBG_EVENT_NAME);
    if (!hDbgEvent)
        return false;

    if (!GetModuleFileNameW(NULL, wszFilePath, _countof(wszFilePath)))
        return false;

    swprintf_s(wszCmdLine, _countof(wszCmdLine), L"\"%s\" %d", wszFilePath, GetCurrentProcessId());

    if (CreateProcessW(NULL, wszCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return WAIT_OBJECT_0 == WaitForSingleObject(hDbgEvent, 0);
    }

    return false;
}

int wmain(int argc, wchar_t** argv)
{
    if (argc < 2)
    {
        if (IsDebugged())
        {
            wprintf(L"[!] Debugger detected!\n");
            ExitProcess(0);
        }
        else
        {
            wprintf(L"[+] No debugger detected.\n");
        }
    }
    else
    {
        DWORD dwParentPid = _wtoi(argv[1]);
        HANDLE hEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, EVENT_SELFDBG_EVENT_NAME);
        if (hEvent && EnableDebugPrivilege())
        {
            if (!DebugActiveProcess(dwParentPid))
                SetEvent(hEvent);
            else
                DebugActiveProcessStop(dwParentPid);
        }
        ExitProcess(0);
    }

    return 0;
}

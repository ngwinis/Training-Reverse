#include <Windows.h>
#include <iostream>

typedef DWORD(WINAPI* TCsrGetProcessId)(VOID);

bool IsDebuggerPresent_OpenProcess()
{
    HMODULE hNtdll = LoadLibraryA("ntdll.dll");
    if (!hNtdll)
        return false;

    auto pfnCsrGetProcessId = (TCsrGetProcessId)GetProcAddress(hNtdll, "CsrGetProcessId");
    if (!pfnCsrGetProcessId)
    {
        FreeLibrary(hNtdll);
        return false;
    }

    DWORD csrssPid = pfnCsrGetProcessId();
    HANDLE hCsr = OpenProcess(PROCESS_ALL_ACCESS, FALSE, csrssPid);

    if (hCsr != NULL)
    {
        CloseHandle(hCsr);
        FreeLibrary(hNtdll);
        return true; // debugger suspected
    }

    FreeLibrary(hNtdll);
    return false;
}

int main()
{
    std::cout << "Checking for debugger using OpenProcess + CsrGetProcessId...\n";

    if (IsDebuggerPresent_OpenProcess())
        std::cout << "[+] Debugger detected!\n";
    else
        std::cout << "[-] No debugger detected.\n";

    return 0;
}

#include <windows.h>
#include <vector>
#include <string>

using namespace std;

const vector<string> vWindowClasses = {
    "antidbg",
    "ID",               // Immunity Debugger
    "ntdll.dll",        // Name of abnormal window
    "ObsidianGUI",
    "OLLYDBG",          // OllyDbg
    "Rock Debugger",
    "SunAwtFrame",      // Java Debugger
    "Qt5QWindowIcon",   // x64dbg
    "WinDbgFrameClass", // WinDbg
    "Zeta Debugger",
};

bool IsDebugged()
{
    for (const auto& sWndClass : vWindowClasses)
    {
        HWND hWnd = FindWindowA(sWndClass.c_str(), NULL);
        if (hWnd != NULL)
        {
            return true;
        }
    }
    return false;
}

int main()
{
    if (IsDebugged())
    {
        MessageBoxA(NULL, "Debugger detected via window class!", "Anti-Debug", MB_ICONWARNING);
        return 1;
    }

    MessageBoxA(NULL, "No debugger detected.", "Anti-Debug", MB_OK);
    return 0;
}

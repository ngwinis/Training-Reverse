#include <windows.h>
#include <iostream>

volatile void TimeWork() {
    volatile double x = 1.0;
    for (int i = 0; i < 500000; ++i) {
        x = x * 1.00001 + i;
        x = x / 1.00001 - i;
    }
}

bool IsDebugged(DWORD64 qwNativeElapsed)
{
    SYSTEMTIME stStart, stEnd;
    FILETIME ftStart, ftEnd;
    ULARGE_INTEGER uiStart, uiEnd;

    GetLocalTime(&stStart);

    TimeWork();

    GetLocalTime(&stEnd);

    if (!SystemTimeToFileTime(&stStart, &ftStart))
        return false;
    if (!SystemTimeToFileTime(&stEnd, &ftEnd))
        return false;

    uiStart.LowPart = ftStart.dwLowDateTime;
    uiStart.HighPart = ftStart.dwHighDateTime;
    uiEnd.LowPart = ftEnd.dwLowDateTime;
    uiEnd.HighPart = ftEnd.dwHighDateTime;

    if (uiStart.QuadPart == 0 || uiEnd.QuadPart == 0) return false;

    return (uiEnd.QuadPart - uiStart.QuadPart) > qwNativeElapsed;
}


int main()
{
    DWORD64 threshold = 5000000;

    if (IsDebugged(threshold))
    {
        std::cout << "Debugger detected." << std::endl;
    }
    else
    {
        std::cout << "No debugger detected." << std::endl;
    }
    return 0;
}
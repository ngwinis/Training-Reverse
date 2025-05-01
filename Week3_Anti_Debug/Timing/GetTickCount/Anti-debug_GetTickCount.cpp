#include <windows.h>
#include <iostream>

volatile void TickWork() {
    volatile long long l = 5;
    for (int i = 0; i < 800000; ++i) {
        l = (l * l) % 1000000007;
        if (l == 0) l = 5;
    }
}

bool IsDebugged(DWORD dwNativeElapsed)
{
    DWORD dwStart = GetTickCount();

    TickWork();

    DWORD dwEnd = GetTickCount();
    DWORD dwDelta = dwEnd - dwStart;

    if (dwEnd < dwStart) {
        return false;
    }

    return dwDelta > dwNativeElapsed;
}

int main()
{
    DWORD threshold = 500;

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
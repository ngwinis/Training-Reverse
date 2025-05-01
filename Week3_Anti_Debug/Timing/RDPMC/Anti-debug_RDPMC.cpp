#include <windows.h>
#include <iostream>
#include <intrin.h>

volatile void RdtscWork() {
    volatile int x = 0;
    for (int i = 0; i < 2000000; ++i) {
        x ^= i;
    }
}

bool IsDebugged(DWORD64 qwNativeElapsed)
{
    ULARGE_INTEGER Start, End;
    DWORD pre_ecx = 0;

    __try {
        __asm
        {
            mov ecx, pre_ecx
            rdtsc
            mov Start.LowPart, eax
            mov Start.HighPart, edx
        }

        RdtscWork();

        __asm
        {
            mov ecx, pre_ecx
            rdtsc
            mov End.LowPart, eax
            mov End.HighPart, edx
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }

    if (Start.QuadPart == 0 || End.QuadPart == 0) return false;

    return (End.QuadPart - Start.QuadPart) > qwNativeElapsed;
}

int main()
{
    DWORD64 threshold = 500000000;

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
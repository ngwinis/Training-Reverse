#include <windows.h>
#include <iostream>

bool IsDebugged()
{
    __try
    {
        __asm xor eax, eax
        __asm int 0x2D
        __asm nop
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

int main()
{
    if (IsDebugged())
        std::cout << "Debugger detected!\n";
    else
        std::cout << "No debugger.\n";
    return 0;
}

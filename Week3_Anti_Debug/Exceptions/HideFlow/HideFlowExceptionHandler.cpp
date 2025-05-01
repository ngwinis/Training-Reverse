#include <Windows.h>
#include <iostream>

void MaliciousEntry()
{
    MessageBoxA(0, "Code execution reached MaliciousEntry", "Hidden Control Flow", 0);
}

void Trampoline2()
{
    __try
    {
        __asm int 3;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        MaliciousEntry();
    }
}

void Trampoline1()
{
    __try
    {
        __asm int 3;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        Trampoline2();
    }
}

int main()
{
    __try
    {
        __asm int 3;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
    {
        Trampoline1();
    }

    return 0;
}

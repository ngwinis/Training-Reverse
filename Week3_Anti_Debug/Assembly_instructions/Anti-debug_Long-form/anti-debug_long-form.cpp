#include <windows.h>
#include <iostream>

bool g_bDebugged = false;

int filter(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{
    if (code == EXCEPTION_BREAKPOINT)
    {
#ifdef _M_IX86
        ep->ContextRecord->Eip += 2;
#else
        ep->ContextRecord->Rip += 2;
#endif
        g_bDebugged = false;
    }
    else
    {
        g_bDebugged = true;
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

bool IsDebugged()
{
    __try
    {
        __asm
        {
            __emit 0xCD
            __emit 0x03
        }
    }
    __except (filter(GetExceptionCode(), GetExceptionInformation()))
    {
        return g_bDebugged;
    }

    return g_bDebugged;
}

int main()
{
    if (IsDebugged())
        MessageBoxA(NULL, "Debugger detected!", "Anti-Debug", MB_OK);
    else
        MessageBoxA(NULL, "No debugger", "Anti-Debug", MB_OK);

    return 0;
}

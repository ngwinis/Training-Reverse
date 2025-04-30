#include <windows.h>
#include <stdio.h>

BOOL IsDebugged_Int3()
{
    __try
    {
        __asm int 3
        return TRUE;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return FALSE;
    }
}

int main()
{
    if (IsDebugged_Int3())
        MessageBoxA(NULL, "Debugger Detected (int3 trap)", "Anti-Debug", MB_OK | MB_ICONWARNING);
    else
        MessageBoxA(NULL, "No Debugger Found", "Anti-Debug", MB_OK | MB_ICONINFORMATION);

    return 0;
}

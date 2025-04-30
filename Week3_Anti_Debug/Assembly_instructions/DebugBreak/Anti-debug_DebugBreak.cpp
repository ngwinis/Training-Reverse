#include <windows.h>
#include <iostream>

bool IsDebugged()
{
    __try
    {
        DebugBreak();
    }
    __except (GetExceptionCode() == EXCEPTION_BREAKPOINT ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
        return false;
    }

    return true;
}

int main()
{
    if (IsDebugged())
        std::cout << "Debugger detected!\n";
    else
        std::cout << "No debugger.\n";

    return 0;
}

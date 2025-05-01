#include <windows.h>
#include <iostream>

bool Check()
{
    __try
    {
        RaiseException(DBG_CONTROL_C, 0, 0, NULL);
        return true;
    }
    __except ((GetExceptionCode() == DBG_CONTROL_C) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
        return false;
    }
}

int main()
{
    if (Check())
    {
        std::cout << "Debugger detected." << std::endl;
    }
    else
    {
        std::cout << "No debugger detected." << std::endl;
    }

    std::cout << "Continuing execution..." << std::endl;
    return 0;
}
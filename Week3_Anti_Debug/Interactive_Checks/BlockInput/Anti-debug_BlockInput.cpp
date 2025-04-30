#include <windows.h>
#include <iostream>

bool IsHooked()
{
    BOOL bFirstResult = FALSE, bSecondResult = FALSE;

    __try
    {
        bFirstResult = BlockInput(TRUE);
        bSecondResult = BlockInput(TRUE);
    }
    __finally
    {
        BlockInput(FALSE);
    }
    return bFirstResult && bSecondResult;
}

int main()
{
    if (IsHooked())
        std::cout << "Debugger detected!\n";
    else
        std::cout << "No debugger detected.\n";

    return 0;
}

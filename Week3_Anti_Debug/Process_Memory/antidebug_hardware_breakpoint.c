#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

bool IsDebugged()
{
    CONTEXT ctx;
    ZeroMemory(&ctx, sizeof(CONTEXT));
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if (!GetThreadContext(GetCurrentThread(), &ctx))
        return false;

    return ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3;
}

int main()
{
    if (IsDebugged()) {
        MessageBoxA(NULL, "Hardware Breakpoint Detected!", "Anti-Debug", MB_OK | MB_ICONWARNING);
    } else {
        MessageBoxA(NULL, "No Debugger Detected.", "Anti-Debug", MB_OK | MB_ICONINFORMATION);
    }

    return 0;
}

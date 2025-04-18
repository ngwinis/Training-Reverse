#include <windows.h>
#include <stdio.h>
#include <winternl.h>

int main() {
#ifdef _WIN64
    PPEB peb = (PPEB)__readgsqword(0x60);
#else
    PPEB peb = (PPEB)__readfsdword(0x30);
#endif

    if (peb->BeingDebugged)
        printf("Debugger detected (manual).\n");
    else
        printf("No debugger (manual).\n");

    if (IsDebuggerPresent())
        printf("Debugger detected (API).\n");
    else
        printf("No debugger (API).\n");

    return 0;
}

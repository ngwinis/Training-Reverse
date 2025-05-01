#include <windows.h>
#include <iostream>
#include <intrin.h>

using namespace std;

LPTOP_LEVEL_EXCEPTION_FILTER g_pPreviousExceptionFilter = nullptr;

LONG WINAPI CustomUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
    if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
        PCONTEXT ctx = pExceptionInfo->ContextRecord;
#ifdef _WIN64
        ctx->Rip += 3;
#else
        ctx->Eip += 3;
#endif
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else {
        if (g_pPreviousExceptionFilter) {
            return g_pPreviousExceptionFilter(pExceptionInfo);
        }
        else {
            return EXCEPTION_CONTINUE_SEARCH;
        }
    }
}

bool CheckIfDebuggerPresentUEF() {
    bool bDebugged = true;
    g_pPreviousExceptionFilter = SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);

    __asm {
        int 3
        jmp near being_debugged
    }
    bDebugged = false;

being_debugged:
    if (g_pPreviousExceptionFilter) {
        SetUnhandledExceptionFilter(g_pPreviousExceptionFilter);
    }
    else {
        SetUnhandledExceptionFilter(NULL);
    }
    return bDebugged;
}

int main() {
    if (CheckIfDebuggerPresentUEF()) {
        cout << "Debugger detected" << endl;
    }
    else {
        cout << "No debugger detected" << endl;
    }
    return 0;
}
// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

extern HINSTANCE hAppInstance;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    BOOL bReturnValue = TRUE;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hAppInstance = hModule;
        MessageBoxA(NULL, "Injected to this process!", "Reflective Dll Injection", MB_OK);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


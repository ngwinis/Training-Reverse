#include "ReflectiveDllInjection.h"

#ifndef _REFLECTIVEDLLINJECTION_LOADLIBRARYN_H
#define _REFLECTIVEDLLINJECTION_LOADLIBRARYN_H

//============================================

DWORD GetReflectiveLoaderOffset(VOID* lpReflectiveDllBuffer);
HANDLE WINAPI LoadRemoteLibraryN(HANDLE hProcess, LPVOID lpBuffer, DWORD dwLength, LPVOID lpParameter);

//============================================

#endif
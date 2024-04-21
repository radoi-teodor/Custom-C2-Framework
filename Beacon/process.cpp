#include "process.h"
#include "core.h"
#include <iostream>


PROCESS_INFO* process::create_process(wchar_t cmd[]) {
    LPSTARTUPINFOW       si;
    PPROCESS_INFORMATION pi;
    BOOL                 success;

    si = new STARTUPINFOW();
    si->cb = sizeof(LPSTARTUPINFOW);

    pi = new PROCESS_INFORMATION();

    SECURITY_ATTRIBUTES saAttr;

    ZeroMemory(&saAttr, sizeof(saAttr));
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE m_hChildStd_OUT_Rd = NULL;
    HANDLE m_hChildStd_OUT_Wr = NULL;

    if (!CreatePipe(&m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &saAttr, 0))
    {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return NULL;
    }

    if (!SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return NULL;
    }

    ZeroMemory(si, sizeof(STARTUPINFO));
    si->cb = sizeof(STARTUPINFO);
    si->hStdError = m_hChildStd_OUT_Wr;
    si->hStdOutput = m_hChildStd_OUT_Wr;
    si->dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(pi, sizeof(PROCESS_INFORMATION));

    success = CreateProcess(
        NULL,
        cmd,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        si,
        pi);

    if (!success) {
        std::cout << "[x] CreateProcess failed." << std::endl;
        return NULL;
    }

    PROCESS_INFO* info = new PROCESS_INFO;
    info->pi = pi;
    info->stdOutRd = m_hChildStd_OUT_Rd;

    // we have to close the write handle, otherwise, the ReadFile will hang stuck
    CloseHandle(m_hChildStd_OUT_Wr);

    return info;
}

PPROCESS_INFORMATION process::create_spoofed_process(wchar_t cmd[], int pid) {
    const DWORD attributeCount = 1;

    LPSTARTUPINFOEXW si = new STARTUPINFOEXW();
    si->StartupInfo.cb = sizeof(STARTUPINFOEXW);

    SIZE_T lpSize = 0;

    InitializeProcThreadAttributeList(
        NULL,
        attributeCount,
        0,
        &lpSize);

    si->lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)malloc(lpSize);

    InitializeProcThreadAttributeList(
        si->lpAttributeList,
        attributeCount,
        0,
        &lpSize);

    HANDLE hParent = OpenProcess(
        PROCESS_CREATE_PROCESS,
        FALSE,
        pid);

    UpdateProcThreadAttribute(
        si->lpAttributeList,
        NULL,
        PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,
        &hParent,
        sizeof(HANDLE),
        NULL,
        NULL);

    PPROCESS_INFORMATION pi = new PROCESS_INFORMATION();

    CreateProcess(
        NULL,
        cmd,
        NULL,
        NULL,
        FALSE,
        EXTENDED_STARTUPINFO_PRESENT,
        NULL,
        NULL,
        &si->StartupInfo, // use the new items from the updated list
        pi);

    DeleteProcThreadAttributeList(si->lpAttributeList);
    free(si->lpAttributeList);

    return pi;
}

void process::close_process(PPROCESS_INFORMATION pi) {
    CloseHandle(pi->hThread);
    CloseHandle(pi->hProcess);
}

LPVOID process::create_page_process(HANDLE p_handle,
    SIZE_T size) { // 4000 bytes -> one page
    LPVOID ptr = VirtualAllocEx(
        p_handle,
        NULL,
        size,
        MEM_COMMIT,
        PAGE_EXECUTE_READWRITE
    );

    if (ptr == NULL) {
        std::cout << "[!] Create Page Failed With Error : " << GetLastError() << std::endl;
    }

    return ptr;
}

PROCESSES* process::enumerate_all_processes() {
    // DLL inclusions
    HMODULE hModule = LoadLibraryA("psapi.dll");
    PVOID pEnumProcesses = GetProcAddress(hModule, "EnumProcesses");
    EnumProcessesFunctionPointer EnumProcesses = (EnumProcessesFunctionPointer)pEnumProcesses;

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return NULL;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    PROCESSES* processes = new PROCESSES;
    processes->process_num = cProcesses;
    processes->processes_id = aProcesses;

    return processes;
}
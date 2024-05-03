#include <vector>
#include <string>
#include "core.h"

#pragma once
class Communicator
{
private:
	std::vector<std::string> commandsToExecute;
	std::string listenerIp;
	int listenerPort;

public:
	void get_commands();

	void execute_commands();

	void send_output(std::string msg);

	friend class HttpCommunicator;
};


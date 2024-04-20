#include <iostream>
#include "core.h"

int main() {
	core c;

	// test cmd execution
	// we need to update the process functions to redirect everything from STDOUT to a pipe (anon or named - TBD) - see this: https://stackoverflow.com/questions/42402673/createprocess-and-capture-stdout @teodor
	c.execute_command("shell whoami /all");
}
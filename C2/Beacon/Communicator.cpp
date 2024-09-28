#include "Communicator.h"
#include "core.h"
#include <string>
#include <vector>

void Communicator::get_commands() {
	// TO BE IMPLEMENTED IN CHILD CLASSES
}

void Communicator::send_output(std::string msg) {
	// TO BE IMPLEMENTED IN CHILD CLASSES
}

void Communicator::execute_commands() {
	if (!this->commandsToExecute.empty()) {
		for (int i = 0; i < this->commandsToExecute.size(); i++) {
			core::instance->execute_command(this->commandsToExecute[i]);
		}
	}
	this->commandsToExecute.clear();

}
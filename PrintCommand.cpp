#include "PrintCommand.h"
#include <iostream>

using namespace std;

PrintCommand::PrintCommand(int pid, string toPrint) : ICommand(pid, PRINT) {
	this->toPrint = toPrint;
}

void PrintCommand::execute() {
	ICommand::execute();

	/*cout << "PID " << this->pid << ":" << this->toPrint << endl;*/
}

string PrintCommand::getToPrint() const {
	return toPrint;
}
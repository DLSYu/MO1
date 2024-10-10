#include "Process.h"
#include <string>
#include "ICommand.h"
#include "PrintCommand.h"
#include <fstream>
using namespace std;

Process::Process(int pid, string processName)
	:pid(pid), processName(processName), cpuCore(-1), commandCounter(0),
	remainingTime(500), processState(READY) {}

void Process::addCommand(ICommand::CommandType command) {
	commandList.push_back(make_shared<PrintCommand>(commandCounter + 1, "Hello World processID: " + to_string(pid)));
}

void Process::executeCommand()  {
	ofstream logFile;
	logFile.open(processName + ".txt", std::ios_base::app);
	if (logFile.is_open()) {
		logFile << "(" << getCurrentTime() << ")" << "  " << "Core: " << cpuCore << "  " << "Hello World from cPID: " <<
			commandList.size() <<  "\n";
		logFile.close();
	}
	commandList.back()->execute();
	commandList.pop_back();
}

bool Process::isFinished() const {
	return processState == FINISHED;
}

int Process::getRemainingTime() const{
	return remainingTime;
}

int Process::getCommandCounter() const {
	return commandCounter;
}

int Process::getLinesOfCode() const {
	return 100;
}

void Process::setCPUCoreID(int coreId) {
	cpuCore = coreId;
}

int Process::getCPUCoreID() const {
	return cpuCore;
}

string Process::getName() const {
	return processName;
}

Process::ProcessState Process::getProcessState() const {
	return processState;
}

void Process::setState(ProcessState processState) {
	this->processState = processState;
}

void Process::initializeCommands() {
	for (int i = 0; i < 10; ++i) {
		addCommand(ICommand::PRINT);
		commandCounter = commandList.size();
	}
}

string Process::getCurrentTime() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	errno_t err;

	err = localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d, %r", &tstruct);

	return buf;
}
#include "Process.h"
#include <string>
#include "ICommand.h"
#include "PrintCommand.h"
#include <fstream>
using namespace std;

Process::Process(int pid, string processName, int linesOfCode, int mem_per_proc)
	:pid(pid), processName(processName), cpuCore(-1), commandCounter(0), linesOfCode(linesOfCode),
	remainingTime(0), processState(READY), currLine(1), timeCreated(getCurrentTime()) {}

void Process::addCommand(ICommand::CommandType command) { // setting cid to 0 muna
	commandList.push_back(make_shared<PrintCommand>(0, "Hello World from " + processName + "!"));
}

void Process::executeCommand() {
	// if print
	//auto printCmd = dynamic_pointer_cast<PrintCommand>(commandList.back());
	//ofstream logFile;
	//string filename = processName + ".txt";

	//// Check if the file already exists
	//bool headersWritten = fileExists(filename);

	//logFile.open(filename, ios_base::app);
	//if (logFile.is_open()) {
	//	// Write headers only if the file is new
	//	if (!headersWritten) {
	//		logFile << "Process name: " << processName << endl
	//			<< "Logs: " << endl << endl;
	//	}

	//	logFile << "(" << getCurrentTime() << ")"
	//		<< "  " << "Core: " << cpuCore
	//		<< "  \"" << printCmd->getToPrint() << "\"" << endl;

	//	logFile.close();
	//}

	commandList.back()->execute();
	commandList.pop_back();
}

bool Process::fileExists(const string& filename) {
	ifstream file(filename);
	return file.good();
}

int Process::getRemainingCommands() {
	return linesOfCode - currLine;
}

int Process::getPID() const {
	return pid;
}

bool Process::isFinished() const {
	return processState == FINISHED;
}

void Process::setCPUCoreID(int coreId) {
	cpuCore = coreId;
}

void Process::setState(ProcessState processState) {
	this->processState = processState;
}

void Process::setCurrLine(int currLine) {
	this->currLine = currLine;
}

int Process::getRemainingTime() const {
	return remainingTime;
}

int Process::getCommandCounter() const {
	return commandCounter;
}

int Process::getLinesOfCode() const {
	return linesOfCode;
}

int Process::getCurrLine() const {
	return currLine;
}

int Process::getCPUCoreID() const {
	return cpuCore;
}

string Process::getName() const {
	return processName;
}

string Process::getTimeCreated() const {
	return timeCreated;
}

Process::ProcessState Process::getProcessState() const {
	return processState;
}

//string Process::getCurrentTime() {
//    time_t now = time(0);
//    struct tm tstruct;
//    char buf[80];
//    errno_t err;
//
//    err = localtime_s(&tstruct, &now);
//    strftime(buf, sizeof(buf), "%m/%d/%Y %I:%M:%S %p", &tstruct);
//
//    return buf;
//}

string Process::getCurrentTime() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	errno_t err;

	err = localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%m/%d/%Y %I:%M:%S %p", &tstruct);

	return buf;
}

void Process::initializeCommands() {
	for (int i = 0; i < getLinesOfCode(); ++i) {
		addCommand(ICommand::PRINT);
		commandCounter = commandList.size();
	}
}


#pragma once
#include <string>
#include <vector>
#include <memory>  
#include "ICommand.h"
using namespace std;

class Process
{	
public:
	enum ProcessState {
		READY,
		RUNNING,
		WAITING,
		FINISHED
	};
	Process(int pid, string processName);
	void addCommand(ICommand::CommandType command);
	void setCPUCoreID(int coreId);
	void setState(ProcessState processState);
	void executeCommand();
	bool isFinished() const;
	int getRemainingTime() const;
	int getCommandCounter() const;
	int getLinesOfCode() const;
	int getCPUCoreID() const;
	string getName() const;
	ProcessState getProcessState() const;

	void initializeCommands();
	string getCurrentTime();

private:
	int cpuCore;
	int pid;
	int linesOfCode;
	int commandCounter;
	int remainingTime;
	string processName;
	ProcessState processState;
	vector<shared_ptr<ICommand>> commandList;
};
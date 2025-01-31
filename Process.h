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
	Process(int pid, string processName, int linesOfCode, int mem_per_proc, int max_per_frame);
	void addCommand(ICommand::CommandType command);
	void setCPUCoreID(int coreId);
	void setState(ProcessState processState);
	void setCurrLine(int currLine);
	void executeCommand();
	int getPID() const;
	bool isFinished() const;
	int getRemainingCommands();
	int getRemainingTime() const;
	int getCommandCounter() const;
	int getLinesOfCode() const;
	int getCPUCoreID() const;
	int getCurrLine() const;
	int getMemPerProc() const;
	int getPagesRequired();

	string getTimeCreated() const;
	string getName() const;
	ProcessState getProcessState() const;

	void initializeCommands();
	string getCurrentTime();
	void setMemoryPointer(void* ptr);
	void* getMemoryPointer() const;

private:
	int cpuCore;
	int pid;
	int currLine;
	int linesOfCode;
	int commandCounter;
	int remainingTime;
	string processName;
	string timeCreated;
	ProcessState processState;
	vector<shared_ptr<ICommand>> commandList;
	bool fileExists(const string& filename);
	int mem_per_proc;
	void* memoryPointer;
	int pagesRequired;
};
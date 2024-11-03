#pragma once
using namespace std;
#include "Console.h"
#include "Process.h"
#include <vector>
#include <string>
#include <memory>

class BaseScreen : public Console
{
public:
	BaseScreen(string consoleName, int PID, int linesOfCode)
		: Console(consoleName),
		attachedProcess(make_shared<Process>(PID, consoleName, linesOfCode)) {}; 
	void onEnabled() override;
	void process() override;
	void display() override;

	void printProcessInfo() const;
	string getConsoleName() const;
	shared_ptr<Process> getProcess() const;
	
	//di ko gets to HAHAHAA
	bool operator== (string key);

private:
	shared_ptr<Process> attachedProcess;
	string consoleName;

	bool refreshed = false;
	string timeCreated;
};


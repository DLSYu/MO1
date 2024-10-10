#pragma once
#include "ICommand.h"
#include <string>

using namespace std;

class PrintCommand : public ICommand
{
public:
	PrintCommand(int pid, string toPrint);
	void execute() override;
	string getToPrint() const;

private:
	string toPrint;

};
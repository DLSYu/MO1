#include "process.h"
#include <iostream>
#include <conio.h>
#include <string>


Process::Process(std::string name)
{
	this->name = name;
	this->timeCreated = getCurrentTime();
}

void Process::displayProcessInfo()
{
	system("cls");
	std::cout << "Process name: " << this->name << std::endl;
	std::cout << "Current line of instruction: " << this->currInstruct << "/" << this->totalInstruct << std::endl;
	std::cout << "Time created: " << this->timeCreated << std::endl;
}

bool Process::operator==(std::string key)
{
	if (this->name == key)
		return true;
	else 
		return false;
}

std::string Process::getCurrentTime()
{
	//attaching a link where i got the code
	// https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	errno_t err;

	err = localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d, %r", &tstruct);

	return buf;
}
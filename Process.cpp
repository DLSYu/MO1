#include "Process.h"
#include <iostream>
#include <string>
#include <random>

Process::Process(std::string name)
{
	this->name = name;
	this->timeCreated = getCurrentTime();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1000, 4000);
	this->pid = dis(gen);
	this->processType = "C+G";
	this->gpuMemoryUsage = "N/A";
}

void Process::displayProcessInfo()
{
	system("cls");
	std::cout << "Process name: " << this->name << std::endl;
	std::cout << "Current line of instruction: " << this->currInstruct << "/" << this->totalInstruct << std::endl;
	std::cout << "Time created: " << this->timeCreated << std::endl;
	std::cout << "Process ID: " << this->pid << std::endl;
	std::cout << "Process Type: " << this->processType << std::endl;
	std::cout << "GPU Memory Usage: " << this->gpuMemoryUsage << std::endl;
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

std::string Process::getName() const {
	return this->name;
}
std::string Process::getTime() const {
	return this->timeCreated;
}
int Process::getPid() const {
	return this->pid;
}
std::string Process::getProcessType() const {
	return this->processType;
}
std::string Process::getGPUMemoryUsage() const {
	return this->gpuMemoryUsage;
}
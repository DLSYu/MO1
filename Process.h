#pragma once
#include <string>
class Process
{
public:
	Process(std::string name);
	void displayProcessInfo();
	bool operator== (std::string key);
	std::string getName() const;
	std::string getTime() const;
	int getPid() const;
	std::string getProcessType() const;
	std::string getGPUMemoryUsage() const;

protected:
private:
	std::string name;
	int currInstruct = 0;
	int totalInstruct = 999;
	int pid;
	std::string processType;
	std::string timeCreated; 
	std::string gpuMemoryUsage;
	std::string getCurrentTime();
};
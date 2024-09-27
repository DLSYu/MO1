#pragma once
#include <string>
class Process
{
public:
	Process(std::string name);
	void displayProcessInfo();
	bool operator== (std::string key);
protected:
private:
	std::string name;
	int currInstruct = 0;
	int totalInstruct = 999;
	std::string timeCreated;

	std::string getCurrentTime();
};
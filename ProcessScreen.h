#pragma once
#include <string>
class ProcessScreen
{
public:
	ProcessScreen(std::string name);
	void displayProcessScreenInfo();
	bool operator== (std::string key);
protected:
private:
	std::string name;
	int currInstruct = 0;
	int totalInstruct = 999;
	std::string timeCreated;

	std::string getCurrentTime();
};
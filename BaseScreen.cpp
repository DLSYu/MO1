using namespace std;
#include <iostream>
#include <string>
#include "BaseScreen.h"
#include "Process.h"


void BaseScreen::onEnabled() {

}

void BaseScreen::process() {

}

void BaseScreen::display() {

}

void BaseScreen::printProcessInfo() const {
	system("cls");
	std::cout << "Process name: " << this->attachedProcess->getName() << std::endl;
	std::cout << "Current line of instruction: " << attachedProcess->getCurrLine() << "/" << attachedProcess->getLinesOfCode() << std::endl;
	std::cout << "Time created: " << this->attachedProcess->getTimeCreated() << std::endl;
}

string BaseScreen::getConsoleName() const {
	return consoleName;
}

shared_ptr<Process> BaseScreen::getProcess() const {
	return attachedProcess;
}

bool BaseScreen::operator==(std::string key)
{
	if (this->name == key)
		return true;
	else
		return false;
}

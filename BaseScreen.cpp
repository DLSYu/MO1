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
	std::cout << "Process: " << this->attachedProcess->getName() << std::endl;
	std::cout << "ID: " << this->attachedProcess->getPID() << std::endl << std::endl;

	if (attachedProcess->getCurrLine() < attachedProcess->getLinesOfCode()) {
		std::cout << "Current instruction line: " << attachedProcess->getCurrLine() << endl;
		std::cout << "Lines of code: " << attachedProcess->getLinesOfCode() << std::endl;
	}
	else
		std::cout << "Finished!" << std::endl;
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

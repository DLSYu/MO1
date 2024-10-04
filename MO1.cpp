#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>

#include "CustomLayout.h"
#include "MarqueeConsole.h"
#include "MarqueeRun.h"
#include "Process.h"

using namespace std;

void titlePage() {
	cout << "  ____ ____   ___  ____  _____ ______   __" << endl;
	cout << " / ___/ ___| / _ \\|  _ \\| ____/ ___\\ \\ / /" << endl;
	cout << "| |   \\___ \\| | | | |_) |  _| \\___ \\\\ V / " << endl;
	cout << "| |___ ___) | |_| |  __/| |___ ___) || |  " << endl;
	cout << " \\____|____/ \\___/|_|   |_____|____/ |_|  " << endl;
}

void introMessage() {
	cout << "Hello, Welcome to CSOPESY commandline!\n";
	cout << "Type 'exit' to exit, 'clear' to clear the screen\n";
}

void createProcess(std::string command, std::vector<Process>& vector) {
	system("cls");
	//create process
	std::string processName = command.substr(10, command.size());
	Process newProcess = Process(processName);
	vector.push_back(newProcess);
	newProcess.displayProcessInfo();
}

/**
* returns the index of the process that matches the key given
* @param string key - name of the process being looked for
* @param std::vector<Process> vector - list of processes being ran
* @returns the index. -1 if not found
*/
int findIndex(std::string key, std::vector<Process> vector) {
	for (int i = 0; i < vector.size(); ++i) {
		if (vector.at(i) == key)
			return i;
	} return -1;
}

/**
* simulates the functionalities of an attached screen. takes consideration of
* exiting the screen, or reattaching a new one.
* @param vector<Process> processVector used to search for the right process
* 
*/
void attachScreen(vector<Process> processVector) {
	bool inScreen = true;
	string command;
	while (inScreen) {
		getline(cin, command);
		if (command == "exit") {
			inScreen = false;
			system("cls");
			command = "";
			titlePage();
			introMessage();
		}
		//if user wants to attach screen
		else if (command.substr(0, 9) == "screen -r") {
			std::string processName = command.substr(10, command.size());
			int index = findIndex(processName, processVector);
			if (index == -1) {
				cout << "Screen not found." << endl;
			}
			//screen found
			else {
				system("cls");
				processVector.at(index).displayProcessInfo();
			}
		} 
		else if (command.substr(0, 9) == "screen -s") {
			createProcess(command, processVector);
		}
		else {
			cout << "Command invalid!" << endl;
		}
	}
}

int main() {
	string command;
	bool systemRunning = true;
	const vector <string> keywords = {"initialize", "scheduler-test", "scheduler-stop", "report-util"};
	bool inScreen = false; // new variable to check if a screen is up
	vector<Process> processVector; // list of vectors

	titlePage();
	introMessage();

	do {

		cout << "Enter a command: ";
		getline(cin, command);

		// Do special case
		if (command == "clear") {
			
			system("cls");
			titlePage();
			introMessage();
		}
		else if (command == "exit") {
			systemRunning = false;
		}
		else if (command == "layout") {
			CustomLayout *custom_layout = new CustomLayout;
			custom_layout->displayLayout(processVector);
		}
		else if (command == "marquee") {
			MarqueeRun *marquee = new MarqueeRun;
			marquee->RunMarquee();
			system("cls");
			titlePage();
			introMessage();

		}
		// Add new screen command
		else if (command.substr(0, 6) == "screen" && command.size() > 9) {
			// reattaching existing screen

			if (command.substr(7, 2) == "-r" && command.substr(9, 1) == " ") {
				std::string processName = command.substr(10, command.size());
				int index = findIndex(processName, processVector);
				if (index == -1) {
					cout << "Screen not found." << endl;
				}
				else {
					inScreen = true;
					system("cls");
					processVector.at(index).displayProcessInfo();
					attachScreen(processVector);
				}
			}
			// creating a new screen
			else if (command.substr(7, 2) == "-s" && command.substr(9, 1) == " ") {
				inScreen = true;
				createProcess(command, processVector);
				attachScreen(processVector);
			} 
			else {
				// TODO: Separate wrong command and incomplete command (ex. "screen -s" inc; "screen -ls" wrong)
				cout << "Command not recognized.\n";
			}
		}
		else {
			cout << "Command not recognized.\n";
		}

	} while (systemRunning);

	return 0;
}


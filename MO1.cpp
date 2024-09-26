#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
#include "Process.h"

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

bool correctCommand(vector <string> keywords, const string& command) {
	return ranges::find(keywords, command) != keywords.end();
	/*return find(keywords.front(), keywords.back(), command) != keywords.back()*/
}

bool correctPosition(const string& keyword, const string& command) {
	return command.rfind(keyword, 0) == 0;
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
	const vector <string> keywords = {"initialize", "scheduler-test", "scheduler-stop", "report-util"};
	bool inScreen = false; // new variable to check if a screen is up
	vector<Process> processVector; // list of vectors

	titlePage();
	introMessage();

	do {

		cout << "Enter a command: ";
		getline(cin, command);

		// Command recognized
		if (correctCommand(keywords, command)) {

			cout << command << " command recognized. Doing something.\n";

		} 
		// Do special case
		else if (command == "exit" || command == "clear") {

			// Windows
			system("cls");
			
			// Mac
			// system("clear");

			titlePage();
			introMessage();
		}
		// Add new screen command
		else if (command.substr(0, 6) == "screen") {
			// reattaching existing screen
			if (command.substr(7, 2) == "-r") {
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
			else if (command.substr(7, 2) == "-s") {
				inScreen = true;
				createProcess(command, processVector);
				attachScreen(processVector);
			}
		}
		else {

			cout << "Command not recognized.\n";
		}

	} while (command != "exit");

	return 0;
}


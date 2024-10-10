#include <iostream>
#include <thread> 
#include <chrono>
#include <queue>
#include <mutex>
#include <vector>

#include "BaseScreen.h"


using namespace std;

int currentPID = 1;
mutex mtx;
queue<BaseScreen> readyQueue;
bool coresAvailable[4] = { true, true, true, true };

vector<shared_ptr<Process>> runningProcesses;
vector<shared_ptr<Process>> finishedProcesses;

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

void createProcessScreen(std::string command, std::vector<BaseScreen>& vector) {
	system("cls");
	//create process
	std::string screenName = command.substr(10, command.size());
	BaseScreen newScreen = BaseScreen(screenName);
	vector.push_back(newScreen);
	newScreen.printProcessInfo();
}

/**
* returns the index of the process that matches the key given
* @param string key - name of the process being looked for
* @param std::vector<Process> vector - list of processes being ran
* @returns the index. -1 if not found
*/
int findIndex(std::string key, std::vector<BaseScreen> vector) {
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
void attachScreen(vector<BaseScreen> processVector) {
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
				processVector.at(index).printProcessInfo();
			}
		}
		else if (command.substr(0, 9) == "screen -s") {
			createProcessScreen(command, processVector);
		}
		else {
			cout << "Command invalid!" << endl;
		}
	}
}


void cpuWorker(int coreId) {
	while (true) {
		std::shared_ptr<Process> process = nullptr;
		bool hasProcess = false;

		{
			lock_guard<mutex> lock(mtx);
			if (!readyQueue.empty()) {
				process = readyQueue.front().getProcess();
				readyQueue.pop();
				process->setCPUCoreID(coreId);
				coresAvailable[coreId] = false;
				hasProcess = true;
				process->setState(Process::RUNNING);
				runningProcesses.push_back(process);
			}
		}

		if (hasProcess) {
			process->initializeCommands();
			for (int i = 1; i <= process->getCommandCounter(); i++) {
				process->setCurrLine(i);
				this_thread::sleep_for(chrono::milliseconds(process->getRemainingTime()));  // add delay
				process->executeCommand();
			}

			process->setState(Process::FINISHED);

			// Mark core as available
			{
				lock_guard<mutex> lock(mtx);
				runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
				finishedProcesses.push_back(process);
				coresAvailable[coreId] = true;
			}
		}

	}
}

void scheduler() {
	while (true) {
		for (int i = 0; i < 4; ++i) {
			if (coresAvailable[i] && !readyQueue.empty()) {
				thread cpuThread(cpuWorker, i);
				cpuThread.detach();
			}
		}
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

int countAvailCores() {
	int count = 0;
	for (int i = 0; i < sizeof(coresAvailable); ++i) {
		if (coresAvailable[i]) {
			count++;
		}
	}
	return count;
}


int main() {
	string command;
	const vector <string> keywords = { "initialize", "scheduler-test", "scheduler-stop", "report-util" };
	bool inScreen = false; // new variable to check if a screen is up
	vector<BaseScreen> processVector; // list of vectors

	titlePage();
	introMessage();

	do {

		cout << "Enter a command: ";
		getline(cin, command);

		// Command recognized
		if (correctCommand(keywords, command)) {

			cout << command << " command recognized. Doing something.\n";

		}
		else if (command == "screen -ls") {
			cout << "Cores Used: " << 4 - countAvailCores() << endl;
			cout << "Cores Available: " << countAvailCores() << endl;
			cout << "--------------------------------------------------------------------" << endl;
			cout << "Running Processes: " << endl;
			for (const auto& process : runningProcesses) {
				cout << process->getName() << "     (" << process->getTimeCreated() << ")     "
					<< "Core: " << process->getCPUCoreID()
					<< "     " << process->getCurrLine() << " / " << process->getLinesOfCode() << endl;
			}
			cout << "\nFinished Processes: " << endl;
			for (const auto& process : finishedProcesses) {
				cout << process->getName() << "     (" << process->getTimeCreated() << ")     "
					<< "Finished     " << process->getCurrLine() << " / " << process->getLinesOfCode() << endl;
			}
			cout << "--------------------------------------------------------------------" << endl;
		}
		else if (command == "scheduler") {
			// Creating some test processes
			cout << "Creating 5 test processes...\n";

			for (int i = 1; i <= 5; ++i) {
				string name = "screen_" + to_string(i);
				BaseScreen s = BaseScreen(name);
				/*auto p = make_shared<Process>(i, "process" + to_string(currentPID));*/
				readyQueue.push(s);
				currentPID++; // Increment the PID Global Variable
			}

			thread schedulerThread(scheduler);
			schedulerThread.detach();
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
					processVector.at(index).printProcessInfo();
					attachScreen(processVector);
				}
			}
			// creating a new screen
			else if (command.substr(7, 2) == "-s") {
				inScreen = true;
				createProcessScreen(command, processVector);
				attachScreen(processVector);
			}
		}
		else {

			cout << "Command not recognized.\n";
		}

	} while (command != "exit");

	return 0;
}
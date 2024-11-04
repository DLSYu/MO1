#include <iostream>
#include <fstream>
#include <sstream>
#include <thread> 
#include <chrono>
#include <queue>
#include <mutex>
#include <vector>

#include "BaseScreen.h"

using namespace std;

// Config File Variables
string fileContent;
int num_cpu;
string scheduler_type;
int quantum_cycles;
int batch_process_freq;
int min_ins;
int max_ins;
int delay_per_exec;
int cpuUtil;


// Global Variables
bool schedulerRunning = false;
int currentPID = 1;
int cpuCycles = 0;
mutex mtx;
queue<shared_ptr<BaseScreen>> readyQueue;
vector<shared_ptr<BaseScreen>> processVector;
vector<bool> coresAvailable;
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

int getNumOfInstructions() {
	return rand() % (max_ins - min_ins + 1) + min_ins;
}

bool correctCommand(vector <string> keywords, const string& command) {
	return ranges::find(keywords, command) != keywords.end();
	/*return find(keywords.front(), keywords.back(), command) != keywords.back()*/
}

bool correctPosition(const string& keyword, const string& command) {
	return command.rfind(keyword, 0) == 0;
}

// Specifically for screen command for creating processes
void createProcessScreen(std::string processName) {
	// Create process
	std::string screenName = processName;
	auto newScreen = make_shared<BaseScreen>(processName, currentPID, getNumOfInstructions());
	currentPID++;
	processVector.push_back(newScreen);
	readyQueue.push(newScreen);
}

/**
* Returns the index of the process that matches the key given
* @param string key - name of the process being looked for
* @param std::vector<Process> vector - list of processes being ran
* @returns the index. -1 if not found
*/
int findIndex(std::string key, std::vector<shared_ptr<BaseScreen>> vector) {
	for (int i = 0; i < vector.size(); ++i) {
		if (vector.at(i)->getProcess()->getName() == key)
			return i;
	}
	return -1;
}

/**
* Simulates the functionalities of an attached screen. Takes consideration of
* exiting the screen, or reattaching a new one.
* @param vector<Process> processVector used to search for the right process
*
*/
void attachScreen(string processName) {
	system("cls");
	bool inScreen = true;
	int index = findIndex(processName, processVector);
	processVector.at(index)->printProcessInfo();

	string command;
	while (inScreen) {
		cout << "\nroot:\\> ";
		getline(cin, command);

		cout << endl;
		// If user wants to attach screen
		if (command == "process-smi") {
			processVector.at(index)->printProcessInfo();
		}
		else if (command == "exit") {
			inScreen = false;
			system("cls");
			command = "";
			titlePage();
			introMessage();
		}
		else {
			cout << "Command invalid!" << endl;
		}
	}
}

void cpuWorker(int coreId) {
	while (true) {
		std::shared_ptr<Process> process = nullptr;
		std::shared_ptr<BaseScreen> baseScreen = nullptr;
		bool hasProcess = false;

		{
			lock_guard<mutex> lock(mtx);
	
			if (!readyQueue.empty()) {
				// Get a shared pointer to BaseScreen from readyQueue
				baseScreen = readyQueue.front();
				readyQueue.pop();

				// Get the Process associated with the BaseScreen
				process = baseScreen->getProcess();
				if (process) {
					process->setCPUCoreID(coreId);
					coresAvailable[coreId] = false;
					hasProcess = true;
					process->setState(Process::RUNNING);
					runningProcesses.push_back(process);
				}
			}
		}

		if (hasProcess) {
			process->initializeCommands();

			if (scheduler_type == "rr") {
				int startLine = process->getCurrLine();
				int endLine = min(startLine + quantum_cycles, process->getLinesOfCode());
				// delay-per-exec
				for (int delay = 0; delay < delay_per_exec; ++delay) {
					this_thread::sleep_for(chrono::milliseconds(delay_per_exec));
					{
						lock_guard<mutex> lock(mtx);
						cpuCycles++;
					}
				}

				// execute start to end commands
				for (int i = startLine; i <= endLine; ++i) {
					process->executeCommand();
					this_thread::sleep_for(chrono::milliseconds(process->getRemainingTime()));
					process->setCurrLine(i);
					{
						lock_guard<mutex> lock(mtx);
						cpuCycles++;
					}
				}

				// check if process not complete
				if (process->getCurrLine() >= process->getLinesOfCode()) {
					lock_guard<mutex> lock(mtx);
					{
						process->setState(Process::FINISHED);
						runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
						finishedProcesses.push_back(process);
					}

				}
				//if process complete
				else {
					{
						lock_guard<mutex> lock(mtx);
						process->setState(Process::WAITING);
						runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
						readyQueue.push(baseScreen); // Re-add the BaseScreen back to the queue
					}
				}
			}
			// fcfs
			else {
				for (int i = 1; i <= process->getCommandCounter(); i++) {
					for (int delay = 0; delay < delay_per_exec; ++delay) {
						this_thread::sleep_for(chrono::milliseconds(delay_per_exec));
					}

					process->setCurrLine(i);
					this_thread::sleep_for(chrono::milliseconds(process->getRemainingTime()));
					process->executeCommand();

					{
						lock_guard<mutex> lock(mtx);
						cpuCycles++;
					}
				}

				{
					lock_guard<mutex> lock(mtx);
					process->setState(Process::FINISHED);
					runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
					finishedProcesses.push_back(process);
				}
			}

			lock_guard<mutex> lock(mtx);
			{
				coresAvailable[coreId] = true;
			}
		}
	}
}

void scheduler() {
	while (true) {
		for (int i = 0; i < num_cpu; ++i) {
			if (coresAvailable[i] && !readyQueue.empty()) {
				thread cpuThread(cpuWorker, i);
				cpuThread.detach();
			}
		}

		// Create new process only when scheduler flag is true
		if (cpuCycles % batch_process_freq == 0 && schedulerRunning == true) {
			string processName = "screen_" + to_string(currentPID);

			// Create new BaseScreen as shared_ptr
			auto newScreen = make_shared<BaseScreen>(processName, currentPID++, getNumOfInstructions());

			{
				unique_lock<std::mutex> lock(mtx);
				readyQueue.push(newScreen);          // Add to readyQueue as shared_ptr
				processVector.push_back(newScreen);  // Also store in processVector as shared_ptr
			}
		}

		// Sleep to control scheduling frequency
		this_thread::sleep_for(chrono::milliseconds(100));
		{
			lock_guard<mutex> lock(mtx);
			cpuCycles++;
		}
	}
}


int countAvailCores() {
	int count = 0;
	for (int i = 0; i < coresAvailable.size(); ++i) {
		if (coresAvailable[i]) {
			count++;
		}
	}
	return count;
}

void readConfigFile() {
	string key;
	string value;

	ifstream ConfigFile("config.txt");

	// Use a while loop together with the getline() function to read the file line by line
	while (getline(ConfigFile, fileContent)) {
		// Output the text from the file
		istringstream iss(fileContent);
		iss >> key >> value;

		if (key == "num-cpu") {
			num_cpu = stoi(value);
		}
		else if (key == "scheduler") {
			value.erase(0, 1); // Remove the first character
			value.erase(value.size() - 1); // Remove the last character
			scheduler_type = value;
		}
		else if (key == "quantum-cycles") {
			quantum_cycles = stoi(value);
		}
		else if (key == "batch-process-freq") {
			if (stoi(value) <= 0) {
				cout << "Invalid batch-process-freq value. Setting to default value 1." << endl;
				batch_process_freq = 1;
			}
			else
				batch_process_freq = stoi(value);
		}
		else if (key == "min-ins") {
			min_ins = stoi(value);
		}
		else if (key == "max-ins") {
			max_ins = stoi(value);
		}
		else if (key == "delay-per-exec") {
			delay_per_exec = stoi(value);
		}
	}

	//Change CPU core size
	coresAvailable = vector<bool>(num_cpu, true);

	// Close the file
	ConfigFile.close();
}

int main() {
	string command;
	const vector <string> keywords = { "initialize", "scheduler-test", "scheduler-stop", "report-util" };
	bool inScreen = false; // new variable to check if a screen is up
	bool isInitialized = false;

	const int screenNameWidth = 13;
	const int dateWidth = 26;
	const int coreWidth = 4;
	const int commandsWidth = 15;
	const int statusWidth = 13;


	titlePage();
	introMessage();

	do {

		cout << "Enter a command: ";
		getline(cin, command);

		//require user to initialize
		if (command == "initialize") {
			isInitialized = true;
			readConfigFile();

			// start scheduler
			thread schedulerThread(scheduler);
			schedulerThread.detach();
			//do file read
			cout << "Configuration has been read" << endl;
			cout << endl;
		}

		//blocks all commands if not initialized
		else if (isInitialized == false) {
			cout << "Please initialize the scheduler first.\n";
		}
		else if (command == "screen -ls") {
			{
				lock_guard<mutex> lock(mtx);
				cpuUtil = ((num_cpu - countAvailCores()) / num_cpu) * 100;
				cout << "CPU Utilization: " << cpuUtil << "%" << endl;
				cout << "Cores Used: " << num_cpu - countAvailCores() << endl;
				cout << "Cores Available: " << countAvailCores() << endl;
				cout << "--------------------------------------------------------------------" << endl;
				cout << "Running Processes: " << endl;
				for (const auto& process : runningProcesses) {
					cout << left << setw(screenNameWidth) << process->getName()
						<< setw(dateWidth) << "(" + process->getTimeCreated() + ")"
						<< setw(coreWidth) << "Core: " << process->getCPUCoreID()
						<< right << setw(commandsWidth) << process->getCurrLine()
						<< " / " << process->getLinesOfCode()
						<< endl;

					cout << left;
				}

				cout << "\nFinished Processes: " << endl;

				for (const auto& process : finishedProcesses) {
					cout << left << setw(screenNameWidth) << process->getName()
						<< setw(dateWidth) << "(" + process->getTimeCreated() + ")"
						<< setw(coreWidth) << "Finished"
						<< right << setw(commandsWidth) << process->getCurrLine()
						<< " / " << process->getLinesOfCode()
						<< endl;

					// Reset to left alignment after the command width
					cout << left;
				}
				cout << "--------------------------------------------------------------------" << endl;
			}
		}
		else if (command == "report-util") {
			{
        lock_guard<mutex> lock(mtx);
				ofstream logFile("csopesy-log.txt");
				cpuUtil = ((num_cpu - countAvailCores()) / num_cpu) * 100;
				logFile << "Cores Used: " << 4 - countAvailCores() << endl;
				logFile << "Cores Available: " << countAvailCores() << endl;
				logFile << "--------------------------------------------------------------------" << endl;
				logFile << "Running Processes: " << endl;

				for (const auto& process : finishedProcesses) {
					logFile << left << setw(screenNameWidth) << process->getName()
						<< setw(dateWidth) << "(" + process->getTimeCreated() + ")"
						<< setw(coreWidth) << "Finished"
						<< right << setw(commandsWidth) << process->getCurrLine()
						<< " / " << process->getLinesOfCode()
						<< endl;

					// Reset to left alignment after the command width
					cout << left;
				}

				logFile << "\nFinished Processes: " << endl;
				for (const auto& process : finishedProcesses) {
					logFile << left << setw(screenNameWidth) << process->getName()
						<< setw(dateWidth) << "(" + process->getTimeCreated() + ")"
						<< setw(coreWidth) << "Finished"
						<< right << setw(commandsWidth) << process->getCurrLine()
						<< " / " << process->getLinesOfCode()
						<< endl;

					// Reset to left alignment after the command width
					cout << left;
				}
				logFile << "--------------------------------------------------------------------" << endl;
				logFile.close();
			}
		}
		else if (command == "scheduler-test") {
			// Creating some test processes
			if (!schedulerRunning) {
				//flag to make new based on cpu cycle
				schedulerRunning = true;
				cout << "Scheduler started...\n";
			}
			else {
				cout << "Scheduler is already running.\n";
			}
		}
		else if (command == "scheduler-stop") {
			// Stop the scheduler
			cout << "Stopping the scheduler...\n";
			schedulerRunning = false;
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
			string processName = command.substr(10, command.size());
			// reattaching existing screen
			if (command.substr(7, 2) == "-r") {
				int index = findIndex(processName, processVector);
				if (index == -1) {
					cout << "Screen not found." << endl;
				}
				else if (processVector.at(index)->getProcess()->getProcessState() == Process::FINISHED) {
					cout << "Process has finished." << endl;
				}
				else {
					inScreen = true;
					attachScreen(processName);
				}
			}
			// creating a new screen
			else if (command.substr(7, 2) == "-s") {
				inScreen = true;
				createProcessScreen(processName);
				attachScreen(processName);
			}
		}
		//for testing
		else {

			cout << "Command not recognized.\n";
		}

	} while (command != "exit");

	return 0;
}
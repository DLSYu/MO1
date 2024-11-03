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


// Global Variables
bool schedulerRunning = false;
int currentPID = 1;
int cpuCycles = 0;
mutex mtx;
queue<BaseScreen> readyQueue;
vector<BaseScreen> processVector;
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

//specifically for screen command for creating processes
void createProcessScreen(std::string processName, std::vector<BaseScreen>& vector) {
	//create process
	std::string screenName = processName;
	BaseScreen newScreen = BaseScreen(screenName, currentPID, getNumOfInstructions());
	currentPID++;
	vector.push_back(newScreen);
	readyQueue.push(newScreen);
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
void attachScreen(vector<BaseScreen> processVector, string processName) {
	system("cls");
	bool inScreen = true;
	int index = findIndex(processName, processVector);
	processVector.at(index).printProcessInfo();

	string command;
	while (inScreen) {
		cout << "\nroot:\\> ";
		getline(cin, command);
		
		cout << endl;
		//if user wants to attach screen
		if (command == "process-smi") {
			processVector.at(index).printProcessInfo();
		}
		else if(command == "exit") {
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

			if (scheduler_type == "rr") {
				int startLine = process->getCurrLine();
				int endLine = min(startLine + quantum_cycles, process->getCommandCounter());

				// Busy-wait for delays_per_exec cycles
				for (int delay = 0; delay < delay_per_exec; ++delay) {
					this_thread::sleep_for(chrono::milliseconds(delay_per_exec));
					{
						lock_guard<mutex> lock(mtx);
						cpuCycles++;
					}
				}

				for (int i = startLine; i <= endLine; ++i) {
					process->setCurrLine(i);
					this_thread::sleep_for(chrono::milliseconds(process->getRemainingTime()));  // Add delay
					process->executeCommand();
					{
						lock_guard<mutex> lock(mtx);
						cpuCycles++;
					}
				}

				if (process->getCurrLine() < process->getCommandCounter()) {
					// Process is not finished, re-add to queue for next round
					{
						lock_guard<mutex> lock(mtx);
						process->setState(Process::WAITING);
						runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
						readyQueue.push(BaseScreen(process->getName(), process->getPID(), process->getCommandCounter() - process->getCurrLine()));
					}
				}
				else {
					{
						lock_guard<mutex> lock(mtx);
						runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
						finishedProcesses.push_back(process);
					}
				}
			}
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

				process->setState(Process::FINISHED);
			}
			
			// Mark core as available
			{
				lock_guard<mutex> lock(mtx);
				if (process->getProcessState() == Process::FINISHED) {
					runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
					finishedProcesses.push_back(process);
				}
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
			BaseScreen newScreen = BaseScreen(processName, currentPID++, getNumOfInstructions());
			readyQueue.push(newScreen);
			processVector.push_back(newScreen);	
		}
		

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
	 // list of vectors

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
			cout << "Cores Used: " << num_cpu - countAvailCores() << endl;
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
		else if (command == "report-util") {
			ofstream logFile("csopesy-log.txt");
			logFile << "Cores Used: " << 4 - countAvailCores() << endl;
			logFile << "Cores Available: " << countAvailCores() << endl;
			logFile << "--------------------------------------------------------------------" << endl;
			logFile << "Running Processes: " << endl;
			for (const auto& process : runningProcesses) {
				logFile << process->getName() << "     (" << process->getTimeCreated() << ")     "
					<< "Core: " << process->getCPUCoreID()
					<< "     " << process->getCurrLine() << " / " << process->getLinesOfCode() << endl;
			}
			logFile << "\nFinished Processes: " << endl;
			for (const auto& process : finishedProcesses) {
				logFile << process->getName() << "     (" << process->getTimeCreated() << ")     "
					<< "Finished     " << process->getCurrLine() << " / " << process->getLinesOfCode() << endl;
			}
			logFile << "--------------------------------------------------------------------" << endl;
			logFile.close();
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
			std::string processName = command.substr(10, command.size());
			int index = findIndex(processName, processVector);
			// reattaching existing screen
			if (command.substr(7, 2) == "-r") {
				if (index == -1) {
					cout << "Screen not found." << endl;
				}
				else if (processVector.at(index).getProcess()->getProcessState() == Process::FINISHED) {
					cout << "Process has finished." << endl;
				}
				else {
					inScreen = true;
					system("cls");
					processVector.at(index).printProcessInfo();
					attachScreen(processVector, processName);
				}
			}
			// creating a new screen
			else if (command.substr(7, 2) == "-s") {
				inScreen = true;
				createProcessScreen(command.substr(10, command.size()), processVector);
				attachScreen(processVector, processName);
			}
		}
		//for testing
		else {

			cout << "Command not recognized.\n";
		}

	} while (command != "exit");

	return 0;
}
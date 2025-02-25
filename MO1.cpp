#include <iostream>
#include <fstream>
#include <sstream>
#include <thread> 
#include <chrono>
#include <queue>
#include <mutex>
#include <vector>

#include "BaseScreen.h"
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"

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
int max_overall_mem;
int mem_per_frame;
int min_mem_per_proc;
int max_mem_per_proc;

// Memory Thigns
string memoryManager = "";
IMemoryAllocator* memoryAllocator;
size_t curr_mem;

int idle = 0;
int active = 0;
int totalcpu = 0;
int pagedin = 0;
int pagedout = 0;

// Global Variables
bool schedulerRunning = false;
int currentPID = 1;
int cpuCycles = 0;
mutex mtx;
condition_variable cv;
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

int getRandomMemPerProc() {
	int value = rand() % (max_mem_per_proc - min_mem_per_proc + 1) + min_mem_per_proc;
	return value;
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
	auto newScreen = make_shared<BaseScreen>(processName, currentPID, getNumOfInstructions(), getRandomMemPerProc(), mem_per_frame);
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

//void memoryFile(int cycle) {
//	lock_guard<mutex> lock(mtx);
//	string fileName = "memory-log" + to_string(cycle) + ".txt";
//	ofstream logFile(fileName);
//	Process tempProcess(0, "temp", 0, 0);
//
//	logFile << "Timestamp:" << tempProcess.getCurrentTime() << endl;
//	logFile << "Number of processes in memory: " << memoryProcesses.size() << endl;
//	logFile << "Total external fragmentation in KB: " << ((max_overall_mem / mem_per_proc) - memoryProcesses.size()) * mem_per_proc << endl;
//	logFile << "---------------------------------end-------------------------------- = " << max_overall_mem << endl;
//
//	int currentAddress = max_overall_mem;
//	for (auto it = memoryProcesses.begin(); it != memoryProcesses.end(); ++it) {
//		logFile << currentAddress << endl;
//		logFile << "Process: " << (*it)->getName() << endl;
//		currentAddress -= mem_per_proc;
//	}
//	while (currentAddress > 0) {
//		logFile << currentAddress << endl;
//		currentAddress -= mem_per_proc;
//	}
//	logFile << "-------------------------------start-------------------------------- = 0" << endl;
//	logFile.close();
//}



void cpuWorker(int coreId) {
	while (true) {
		std::shared_ptr<Process> process = nullptr;
		std::shared_ptr<BaseScreen> baseScreen = nullptr;
		bool hasProcess = false;

		{
			unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [coreId] { return !readyQueue.empty() || !schedulerRunning; });

			if (!readyQueue.empty()) {
				// Get a shared pointer to BaseScreen from readyQueue
				baseScreen = readyQueue.front();
				readyQueue.pop();
				// Get the Process associated with the BaseScreen
				process = baseScreen->getProcess();
				// Check if there is space in memory or if the process is already in memory
				//	get ptr from memoryAllocator->allocate(process->getMemPerProc())
				// 	put that variable in the if condition below
				if (void* memPtr = memoryAllocator->allocate(process)) {
					if (process) {
						// Set the memory pointer
						process->setMemoryPointer(memPtr);
						process->setCPUCoreID(coreId);
						coresAvailable[coreId] = false;
						hasProcess = true;
						process->setState(Process::RUNNING);
						runningProcesses.push_back(process);
					}
				}
				else {
					// If no space in memory, re-add the BaseScreen back to the queue
					//cout << "No space in memory for process " << process->getName() << endl;
					readyQueue.push(baseScreen);
				}
			}
		}

		if (hasProcess) {
			process->initializeCommands();

			if (scheduler_type == "rr") {
				// Round Robin scheduling
				int startLine = process->getCurrLine();
				int endLine = min(startLine + quantum_cycles, process->getLinesOfCode());

				for (int delay = 0; delay < delay_per_exec; ++delay) {
					this_thread::sleep_for(chrono::milliseconds(delay_per_exec));
					{
						lock_guard<mutex> lock(mtx);
						cpuCycles++;
					}
				}

				for (int i = startLine; i <= endLine; ++i) {
					process->executeCommand();
					this_thread::sleep_for(chrono::milliseconds(process->getRemainingTime()));
					process->setCurrLine(i);
					{
						lock_guard<mutex> lock(mtx);
						cpuCycles++;
					}
				}

				if (process->getCurrLine() >= process->getLinesOfCode()) {
					lock_guard<mutex> lock(mtx);
					{
						process->setState(Process::FINISHED);
						runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
						memoryAllocator->deallocate(process->getMemoryPointer(), process); // Corrected line
						finishedProcesses.push_back(process);
					}
				}
				else {
					{
						lock_guard<mutex> lock(mtx);
						process->setState(Process::READY);
						runningProcesses.erase(remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
						memoryAllocator->deallocate(process->getMemoryPointer(), process); // Corrected line
						readyQueue.push(baseScreen);
					}
				}
			}
			else {
				// fcfs
				for (int i = process->getCurrLine(); i <= process->getLinesOfCode(); ++i) {
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
					memoryAllocator->deallocate(process->getMemoryPointer(), process); 
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
	vector<std::thread> workerThreads;
	for (int i = 0; i < num_cpu; ++i) {
		workerThreads.emplace_back(cpuWorker, i);
	}

	while (true) {
		/*for (int i = 0; i < num_cpu; ++i) {
			if (coresAvailable[i] && !readyQueue.empty()) {
				thread cpuThread(cpuWorker, i);
				cpuThread.detach();
			}
		}*/

		// Create new process only when scheduler flag is true
		if (cpuCycles % batch_process_freq == 0 && schedulerRunning == true) {
			string processName = "screen_" + to_string(currentPID);

			// Create new BaseScreen as shared_ptr
			auto newScreen = make_shared<BaseScreen>(processName, currentPID++, getNumOfInstructions(), getRandomMemPerProc(), mem_per_frame);
			{
				unique_lock<std::mutex> lock(mtx);
				readyQueue.push(newScreen);          // Add to readyQueue as shared_ptr
				processVector.push_back(newScreen);  // Also store in processVector as shared_ptr
				cv.notify_one();
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
		else if (key == "max-overall-mem") {
			max_overall_mem = stoi(value);
		}
		else if (key == "mem-per-frame") {
			mem_per_frame = stoi(value);
		}
		else if (key == "min-mem-per-proc") {
			min_mem_per_proc = stoi(value);
		}
		else if (key == "max-mem-per-proc") {
			max_mem_per_proc = stoi(value);
		}
	}

	//Change CPU core size
	coresAvailable = vector<bool>(num_cpu, true);

	// Set memory manager type

	if (max_overall_mem == mem_per_frame) {
		memoryManager = "FLAT_MEMORY";
		memoryAllocator = new FlatMemoryAllocator(max_overall_mem);
	}
	else{
		memoryManager = "PAGING";
		memoryAllocator = new PagingAllocator(max_overall_mem, mem_per_frame);
	}
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
				cpuUtil = (int)(((float)(num_cpu - countAvailCores()) / (float)num_cpu) * 100);
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
				cpuUtil = (int) ( ( (float) (num_cpu - countAvailCores()) / (float) num_cpu) * 100);
				logFile << "CPU Utilization: " << cpuUtil << "%" << endl;
				logFile << "Cores Used: " << num_cpu - countAvailCores() << endl;
				logFile << "Cores Available: " << countAvailCores() << endl;
				logFile << "--------------------------------------------------------------------" << endl;
				logFile << "Running Processes: " << endl;
				for (const auto& process : runningProcesses) {
					logFile << left << setw(screenNameWidth) << process->getName()
						<< setw(dateWidth) << "(" + process->getTimeCreated() + ")"
						<< setw(coreWidth) << "Core: " << process->getCPUCoreID()
						<< right << setw(commandsWidth) << process->getCurrLine()
						<< " / " << process->getLinesOfCode()
						<< endl;

					// Reset to left alignment after the command width
					logFile << left;
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
					logFile << left;
				}
				logFile << "--------------------------------------------------------------------" << endl;
				logFile.close();
			}
		}
		else if (command == "process-smi") {
			cpuUtil = (int) (((float) (num_cpu - countAvailCores()) / (float) num_cpu) * 100);
			curr_mem = memoryAllocator->getAllocatedSize();
			double memUtil = static_cast<double>(curr_mem) / static_cast<double>(max_overall_mem) * 100;

			cout << "--------------------------------------------------------------------" << endl;
			cout << "|           PROCESS-SMI V01.00         Driver Version 01.00        |" << endl;
			cout << "--------------------------------------------------------------------" << endl;
			cout << "CPU Util: " << cpuUtil << "%" << endl;
			cout << "Memory Usage: " << curr_mem << "kB / " << max_overall_mem << "kB" << endl;
			cout << "Memory Util: " << memUtil << "%" << endl;
			cout << "====================================================================" << endl;
			cout << "Running proccesses and memory usage: " << endl;
			cout << "--------------------------------------------------------------------" << endl;
			for (const auto& process : runningProcesses) {
				cout << "Process Name: " << process->getName() << ", Memory Usage: " << process->getMemPerProc() << "kB" << endl;
			}
			cout << "--------------------------------------------------------------------" << endl;


		}
		else if (command == "vmstat") {
			curr_mem = memoryAllocator->getAllocatedSize();
			size_t free_mem = static_cast<size_t>(max_overall_mem - curr_mem);
			idle += 5;
			active += 10;
			totalcpu += 15;
			pagedin += 5;
			pagedout += 10;
			cout << max_overall_mem << " kB total memory" << endl;
			cout << curr_mem << " kB used memory" << endl;
			cout << free_mem << " kB free memory" << endl;
			cout << idle << " idle cpu ticks" << endl;
			cout << active << " active cpu ticks" << endl;
			cout << totalcpu << " total cpu ticks" << endl;
			cout << pagedin << " num paged in" << endl;
			cout << pagedout << " num paged out" << endl;
		}
		else if (command == "readyqueue") {
			cout << readyQueue.size() << " processes in the ready queue." << endl;
		}
		else if (command == "vismem") {
			cout << memoryAllocator->visualizeMemory() << endl;
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
#include<iostream>
#include<cstdlib>
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



int main() {
	string command;

	titlePage();
	introMessage();

	do {
		cout << "Enter a command: ";
		cin >> command;

		// Command recognized
		if (command == "initialize" || command == "screen" || command == "scheduler-test" || command == "scheduler-stop" ||
			command == "report-util" || command == "clear" || command == "exit")
		{
			cout << command << " command recognized. Doing something.\n";
		}

		// Do special case

		if (command == "clear") {
			// Windows
			system("cls");
			
			// Mac
			// system("clear")

			titlePage();
			introMessage();
		}
	} while (command != "exit");

	return 0;
}


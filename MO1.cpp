#include <iostream>
#include <cstdlib>
#include <vector>
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

bool correctCommand(vector <string> keywords, const string& command) {
	return ranges::find(keywords, command) != keywords.end();
}

bool correctPosition(const string& keyword, const string& command) {
	return command.rfind(keyword, 0) == 0;
}



int main() {
	string command;
	const vector <string> keywords = {"initialize", "screen", "scheduler-test", "scheduler-stop", "report-util", "clear", "exit"};

	titlePage();
	introMessage();

	do {

		cout << "Enter a command: ";
		getline(cin, command);

		// Command recognized
		if (correctCommand(keywords, command)) {

			cout << command << " command recognized. Doing something.\n";

		} else {

			cout << "Command not recognized.\n";
		}

		// Do special case
		if (correctCommand(keywords, command) && correctPosition(keywords[5], command)) {

			// Windows
			// system("cls");
			
			// Mac
			system("clear");

			titlePage();
			introMessage();
		}
	} while (command != "exit");

	return 0;
}


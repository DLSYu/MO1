#include "CustomLayout.h"

#include <iostream>
#include <vector>

#include "Process.h"
using namespace std;

CustomLayout::CustomLayout() {

}

void CustomLayout::displayLayout(vector<Process> process) {
	system("cls");
	cout << process[0].getCurrentTime() << endl;
	cout << "+-------------------------------------------------------------------------------------------+" << endl;
	cout << "| NVIDIA-SMI 551.86                 Driver Version: 551.86           CUDA Version: 12.4     |" << endl;
	cout << "+-----------------------------------------+--------------------------+----------------------+" << endl;
	cout << "| GPU  Name                     TCC/WDDM  | Bus-Id            Disp.A | Volatile Uncorr. ECC |" << endl;
	cout << "| Fan  Temp  Perf       Pwr : Usage / Cap |             Memory-Usage | GPU-Util  Compute M. |" << endl;
	cout << "|                                         |                          |               MIG M. |" << endl;
	cout << "|=========================================+==========================+======================|" << endl;
	cout << "|   0  NVIDIA GeForce GTX 1080      WDDM  |     00000000:26:00.0  On |                  N/A |" << endl;
	cout << "| 28%   37C    P8             11W /  180W |       701MiB /   8192MiB |      0%      Default |" << endl;
	cout << "|                                         |                          |                  N/A |" << endl;
	cout << "+-----------------------------------------+--------------------------+----------------------+" << endl << endl;

	cout << "+-------------------------------------------------------------------------------------------+" << endl;
	cout << "| Processes:                                                                                |" << endl;
	cout << "|  GPU   GI   CI       PID    Type   Process name                                GPU Memory |" << endl;
	cout << "|        ID   ID                                                                 Usage      |" << endl;
	cout << "|=========================================+==========================+======================|" << endl;
	
	for (const Process& proc : process) {
		int length = proc.getName().length();
		int padding = 6 + (38 - length);
		cout << "|    0   N/A  N/A     ";
		cout << proc.getPid() << "     ";
		cout << proc.getProcessType() << "   ";
		if (padding < 6) {
			cout << "..." <<proc.getName().substr(length-35, 35) << "      ";
		} else {
			cout << proc.getName() << string(padding, ' ');
		}
		cout << proc.getGPUMemoryUsage() << "        |" << endl;
	}

	cout << "+-------------------------------------------------------------------------------------------+" << endl;
}

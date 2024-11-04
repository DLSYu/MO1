#pragma 
using namespace std;
#include <string>

class Console
{
public:
	string name;

	Console(string name) : name(name) {};

	virtual void onEnabled() = 0;
	virtual void display() = 0;
	virtual void process() = 0;

	string getName() {
		return name;
	}
};


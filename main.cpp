// (c) Carey Nachenberg
#include "test.h"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
	string filename;

	switch (argc)
	{
	case 1:
		cout << "Enter test script file name: ";
		getline(cin, filename);
		break;
	case 2:
		filename = argv[1];
		break;
	default:
		cout << "Usage: " << argv[0] << " scriptname" << endl;
		return 1;
	}

	Test t;
	string problem;

	if (!t.loadAndRunTestScript(filename, problem))
	{
		cout << "Problem running script: " << problem << endl;
		return 1;
	}
}

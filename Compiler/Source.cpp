#include "Token.h"
#include "Scanner.h"

#include <vector>
#include <iostream>

using namespace std;

// Main
int main(int argc, char *argv[]) {

	const char* filePath = "C://Users//guita//Documents//Compiler//testPgms//correct//test_program_minimal.src"; // filepath to read

	Scanner inputScanner;
	inputScanner.scanIn(filePath);

	vector<token> tokens = inputScanner.getTokens();

	//for (int i = 0; i < tokens.size(); i++) {
	//	cout << tokens[i].t_type << endl;
	//}

}
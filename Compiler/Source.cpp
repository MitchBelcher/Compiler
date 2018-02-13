/*

This cpp file contains the main entry point to the compiler program, setting the filepath, creating the initial scanner instance

*/

#include "Token.h"
#include "Scanner.h"
#include "Parser.h"

#include <vector>
#include <iostream>

using namespace std;

// Main
int main(int argc, char *argv[]) {

	const char* filePath = "C://Users//guita//Documents//Compiler//testPgms//correct//test_program_minimal.src"; // Filepath to read

	Parser fileParse(filePath);
	fileParse.parseFile();

	//Scanner inputScanner; // Create Scanner instance

	// Print out errors
	for (int i = 0; i < fileParse.ResultOfParse.size(); i++) {
		cout << fileParse.ResultOfParse[i].lineNumber << '\t' << fileParse.ResultOfParse[i].parseError << endl;
	}
	if (fileParse.ResultOfParse.size() == 0) {
		cout << "No parse errors found!" << endl;
	}

	//inputScanner.scanIn(filePath);

	//vector<token> tokens = inputScanner.getTokens();

	//for (int i = 0; i < tokens.size(); i++) {
	//	cout << tokens[i].t_type << endl;
	//}

	//cin.get();
}
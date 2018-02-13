/*

This cpp file contains the main entry point to the compiler program, setting the filepath, creating the initial scanner instance

*/
#include "Errors.h"

#include "Token.h"
#include "Scanner.h"
#include "Parser.h"

#include <vector>
#include <iostream>

using namespace std;

vector<ParsingError> ResultOfParse = {};
vector<ScannerError> ResultOfScan = {};

// Main
int main(int argc, char *argv[]) {

	//const char* filePath = "C://Users//guita//Documents//Compiler//testPgms//correct//test_program_minimal.src"; // Filepath to read
	const char* filePath = "..//testPgms//correct//test1b.src"; // Filepath to read

	Parser fileParse(filePath);
	fileParse.parseFile();

	//Scanner inputScanner; // Create Scanner instance

	// Print out scanner errors
	for (int i = 0; i < ResultOfScan.size(); i++) {
		cout << "Line #: " << ResultOfScan[i].lineNumber << '\t' << "-- " << ResultOfScan[i].scanError << " --" << '\t' << "-- Error found at/near string: " << ResultOfScan[i].tokenName << endl;
	}
	if (ResultOfScan.size() == 0) {
		cout << "No scan errors found!" << endl;
	}

	// Print out parse errors
	for (int i = 0; i < ResultOfParse.size(); i++) {
		cout << "Line #: " << ResultOfParse[i].lineNumber << '\t' << "-- " << ResultOfParse[i].parseError << " --" << '\t' << "-- Error found at/near string: " << ResultOfParse[i].tokenName << endl;
	}
	if (ResultOfParse.size() == 0) {
		cout << "No parse errors found!" << endl;
	}

	//inputScanner.scanIn(filePath);

	//vector<token> tokens = inputScanner.getTokens();

	//for (int i = 0; i < tokens.size(); i++) {
	//	cout << tokens[i].t_type << endl;
	//}
}
/*

This cpp file contains the main entry point to the compiler program, setting the filepath, creating the initial scanner instance

*/
#include "Errors.h"

#include "Token.h"
#include "Scanner.h"
#include "Parser.h"
#include "SymTable.h"

#include <vector>
#include <iostream>

using namespace std;

vector<ParsingError> ResultOfParse = {};
vector<ScannerError> ResultOfScan = {};
vector<SymbolError> ResultOfSymbol = {};

// Main
int main(int argc, char *argv[]) {

	const char* filePath = "..//testPgms//correct//test1b.src"; // Filepath to read

	SymTable symbolTable;

	Parser fileParse(filePath, symbolTable);
	fileParse.parseFile();

	//symbolTable.OpenScope();

	//symbolTable.CloseScope();

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

	// Print out symbol errors
	for (int i = 0; i < ResultOfSymbol.size(); i++) {
		cout << ResultOfSymbol[i].symbolError << " --" << '\t' << "-- Error found at/near string: " << ResultOfSymbol[i].idString << endl;
	}
	if (ResultOfSymbol.size() == 0) {
		cout << "No parse errors found!" << endl;
	}
}
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

// Reset all error vectors
vector<ParsingError> ResultOfParse = {};
vector<ScannerError> ResultOfScan = {};
vector<SymbolError> ResultOfSymbol = {};

// Main
int main(int argc, char *argv[]) {

	//const char* filePath = "..//testPgms//correct//vectorOps.src"; // Filepath to read
	const char* filePath = "..//testPgms//incorrect//parser//logical.src"; // Filepath to read

	SymTable symbolTable; // Create new blank symbol table

	Parser fileParse(filePath, symbolTable);	// Create parser with set filepath and new blank symbol table
	fileParse.parseFile();						// Parse the input file

	// Print out scanner errors
	if (ResultOfScan.size() != 0) {
		for (int i = 0; i < ResultOfScan.size(); i++) {
			cout << "Line #: " << ResultOfScan[i].lineNumber << '\t' << "-- " << ResultOfScan[i].scanError << " --" << '\t' << '\t' << "-- Error found at/near token: " << ResultOfScan[i].tokenName << endl;
		}
	}
	else {
		cout << "No lexer errors found!" << endl;
	}

	// Print out parse errors
	if (ResultOfParse.size() != 0) {
		for (int i = 0; i < ResultOfParse.size(); i++) {
			cout << "Line #: " << ResultOfParse[i].lineNumber << '\t' << "-- " << ResultOfParse[i].parseError << " --" << '\t' << '\t' << "-- Error found at/near token: " << ResultOfParse[i].tokenName << endl;
		}
	}
	else {
		cout << "No parse errors found!" << endl;
	}

	// Print out symbol errors
	if (ResultOfSymbol.size() != 0) {
		for (int i = 0; i < ResultOfSymbol.size(); i++) {
			cout << ResultOfSymbol[i].symbolError << " --" << '\t' << '\t' << "-- Error found at/near token: " << ResultOfSymbol[i].idString << endl;
		}
	}
	else {
		cout << "No symbol table errors found!" << endl;
	}
}
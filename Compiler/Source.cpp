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

const char* filePath = "";

// Reset all error vectors
vector<ParsingError> ResultOfParse = {};
vector<ScannerError> ResultOfScan = {};
vector<SymbolError> ResultOfSymbol = {};

vector<string> correctPrgms = { "iterativeFib", "logicals", "math", "multipleProcs", "resursiveFib", "source", "test_heap", "test_program_array", "test_program_minimal",  "test1", "test1b", "test2", "vectorOps" };
vector<string> incorrectPrgms = { "invalidChars", "badSource", "iterativeFib", "logical", "recursiveFib", "vectorOps", "assignBoolAndChar", "assignFloatToInt", "test1",  "test2", "test3" };

string relCorrectPath = "..//testPgms//correct//";
string relIncorrectPath = "..//testPgms//incorrect//";

void mainMenu() {
	cout << "Please select your option: " << endl;
	cout << "1: Correct Programs" << endl;
	cout << "2: Incorrect Programs" << endl;
	cout << "3: EXIT" << endl << endl;
}

void makeCorrectMenu() {
	cout << "Please select the program you wish to run: " << endl;
	for (int i = 0; i < correctPrgms.size(); i++) {
		cout << i << ": " << correctPrgms[i] << endl;
	}

	cout << correctPrgms.size() << ": " << "Return to main menu" << endl << endl;
}

void makeIncorrectMenu() {
	cout << "Please select the program you wish to run: " << endl;
	for (int i = 0; i < incorrectPrgms.size(); i++) {
		cout << i << ": " << incorrectPrgms[i] << endl;
	}

	cout << incorrectPrgms.size() << ": " << "Return to main menu" << endl << endl;
}

void checkErrors() {
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
		cout << "No symbol table errors found!" << endl << endl;
	}
}

void doStuff() {
	SymTable symbolTable; // Create new blank symbol table

	Parser fileParse(filePath, symbolTable);	// Create parser with set filepath and new blank symbol table
	fileParse.beginParsingFile();				// Parse the input file

	checkErrors();
}

string setFilePath(bool correct, string fileToUse) {
	string workingString = "";

	if (correct) {
		workingString = relCorrectPath + fileToUse + ".src";
	}
	else {
		workingString = relIncorrectPath + fileToUse + ".src";
	}

	return workingString;
}

// Main
int main(int argc, char *argv[]) {

	int mainMenuSelection;
	int correctMenuSelection;
	int incorrectMenuSelection;
	bool exit = false;
	bool goBack = false;
	string tempPath = "";

	string selectedFile = "";

	while (!exit) {
		mainMenu();
		cin >> mainMenuSelection;

		while (!goBack) {
			switch (mainMenuSelection) {
				case 1:
					makeCorrectMenu();
					cin >> correctMenuSelection;

					switch (correctMenuSelection) {
						case 0:
							tempPath = setFilePath(true, correctPrgms[0]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 1:
							tempPath = setFilePath(true, correctPrgms[1]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 2:
							tempPath = setFilePath(true, correctPrgms[2]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 3:
							tempPath = setFilePath(true, correctPrgms[3]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 4:
							tempPath = setFilePath(true, correctPrgms[4]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 5:
							tempPath = setFilePath(true, correctPrgms[5]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 6:
							tempPath = setFilePath(true, correctPrgms[6]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 7:
							tempPath = setFilePath(true, correctPrgms[7]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 8:
							tempPath = setFilePath(true, correctPrgms[8]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 9:
							tempPath = setFilePath(true, correctPrgms[9]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 10:
							tempPath = setFilePath(true, correctPrgms[10]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 11:
							tempPath = setFilePath(true, correctPrgms[11]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 12:
							tempPath = setFilePath(true, correctPrgms[12]);
							filePath = tempPath.c_str();
							cout << "RUNNING: " << filePath << endl;
							doStuff();
							break;

						case 13:
							goBack = true;
							break;
					}
					break;

				case 2:
					makeIncorrectMenu();
					cin >> incorrectMenuSelection;

					switch (incorrectMenuSelection) {

					}
					break;

				case 3:
					exit = true;
					break;

				default:
					cout << "Bad input, try again" << endl << endl;
					break;
				}
		}

	}

	//const char* filePath = "..//testPgms//correct//iterativeFib.src"; // Filepath to read
}
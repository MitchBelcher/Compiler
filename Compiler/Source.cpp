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

// Initialize all error vectors
vector<ParsingError> ResultOfParse = {};
vector<ScannerError> ResultOfScan = {};
vector<SymbolError> ResultOfSymbol = {};

// Set up vectors for program filenames
vector<string> correctPrgms = { "iterativeFib", "logicals", "math", "multipleProcs", "recursiveFib", "source", "test_heap", "test_program_array", "test_program_minimal",  "test1", "test1b", "test2", "vectorOps" };
vector<string> incorrectPrgms = { "invalidChars", "badSource", "iterativeFib", "logical", "vectorOps", "assignBoolAndChar", "test1",  "test2", "test3" };

// Set up strings for relative filepaths for programs
string relCorrectPath = "..//testPgms//correct//";
string relIncorrectPath = "..//testPgms//incorrect//";

// Function to print the main menu
void mainMenu() {
	cout << "Please select your option: " << endl;
	cout << "1: Correct Programs" << endl;
	cout << "2: Incorrect Programs" << endl;
	cout << "3: Type Your Own Filepath" << endl;
	cout << "4: EXIT" << endl << endl;
}

// Function to print the menu for the correct programs vector
void makeCorrectMenu() {
	cout << "Please select the program you wish to run: " << endl;
	for (int i = 0; i < correctPrgms.size(); i++) {
		cout << i << ": " << correctPrgms[i] << endl;
	}

	cout << correctPrgms.size() << ": " << "Return to main menu" << endl << endl;
}

// Function to print the menu for the incorrect programs vector
void makeIncorrectMenu() {
	cout << "Please select the program you wish to run: " << endl;
	for (int i = 0; i < incorrectPrgms.size(); i++) {
		cout << i << ": " << incorrectPrgms[i] << endl;
	}

	cout << incorrectPrgms.size() << ": " << "Return to main menu" << endl << endl;
}

// Function to check the error vectors and print appropriate messages to the screen
void checkErrors() {

	// Print out scanner errors
	if (ResultOfScan.size() != 0) {
		for (int i = 0; i < ResultOfScan.size(); i++) {
			cout << "Line #: " << ResultOfScan[i].lineNumber << '\t' << "-- " << ResultOfScan[i].scanError << " --" << '\t' << '\t' << "-- Error found at/near token: " << ResultOfScan[i].tokenName << endl;

			if (i == ResultOfScan.size() - 1) {
				cout << endl;
			}
		}
	}
	else {
		cout << "No lexer errors found!" << endl << endl;
	}

	// Print out parse errors
	if (ResultOfParse.size() != 0) {
		for (int i = 0; i < ResultOfParse.size(); i++) {
			cout << "Line #: " << ResultOfParse[i].lineNumber << '\t' << "-- " << ResultOfParse[i].parseError << " --" << '\t' << '\t' << "-- Error found at/near token: " << ResultOfParse[i].tokenName << endl;

			if (i == ResultOfParse.size() - 1) {
				cout << endl;
			}
		}
	}
	else {
		cout << "No parse errors found!" << endl << endl;
	}

	// Print out symbol errors
	if (ResultOfSymbol.size() != 0) {
		for (int i = 0; i < ResultOfSymbol.size(); i++) {
			cout << ResultOfSymbol[i].symbolError << " --" << '\t' << '\t' << "-- Error found at/near token: " << ResultOfSymbol[i].idString << endl;

			if (i == ResultOfSymbol.size() - 1) {
				cout << endl;
			}
		}
	}
	else {
		cout << "No symbol table errors found!" << endl;
	}

	cout << "-------------------------------------" << endl << endl;
}

// Function to clear out the error vectors
void clearErrors() {
	ResultOfParse.clear();
	ResultOfScan.clear();
	ResultOfSymbol.clear();
}

// Function to handle the actual Parser call and error logging, etc...
void doStuff() {
	SymTable symbolTable;	// Create new blank symbol table
	clearErrors();			// Clear any errors

	Parser fileParse(filePath, symbolTable);	// Create parser with set filepath and new blank symbol table
	fileParse.beginParsingFile();				// Parse the input file

	checkErrors();	// Check for errors
}

// Function to set the filepath from the main menu options
string setFilePath(bool correct, string fileToUse) {
	string workingString = "";

	// If we're in the correct programs menu, use that path, otherwise, use the incorrect programs path
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
	bool quit = false;
	bool goBack = false;
	string tempPath = "";
	string fileString = "";

	while (!quit) {
		goBack = false;
		mainMenu();
		cin >> mainMenuSelection;

		while (!goBack) {
			switch (mainMenuSelection) {
				case 1:
					makeCorrectMenu();
					cin >> correctMenuSelection;

					if (correctMenuSelection == correctPrgms.size()) {
						goBack = true;
					}
					else if (correctMenuSelection >= 0 && correctMenuSelection < correctPrgms.size()) {
						tempPath = setFilePath(true, correctPrgms[correctMenuSelection]);
						filePath = tempPath.c_str();
						cout << "-------------------------------------" << endl;
						cout << "RUNNING: " << filePath << endl;
						cout << "-------------------------------------" << endl;
						doStuff();
					}
					else {
						cout << "Bad input, try again" << endl << endl;
					}
					break;

				case 2:
					makeIncorrectMenu();
					cin >> incorrectMenuSelection;

					if (incorrectMenuSelection == incorrectPrgms.size()) {
						goBack = true;
					}
					else if (incorrectMenuSelection >= 0 && incorrectMenuSelection < incorrectPrgms.size()) {
						tempPath = setFilePath(false, incorrectPrgms[incorrectMenuSelection]);
						filePath = tempPath.c_str();
						cout << "-------------------------------------" << endl;
						cout << "RUNNING: " << filePath << endl;
						cout << "-------------------------------------" << endl;
						doStuff();
					}
					else {
						cout << "Bad input, try again" << endl << endl;
					}
					break;

				case 3:
					cout << "Type in the filepath for the file you wish to compile" << endl;
					cout << "EXAMPLE:  C://Documents//testprogram.src" << endl;
					cout << "If this path is wrong, the compiler will crash" << endl << endl;
					cout << "Alternatively, enter '-1' to return to the main menu" << endl;
					cin >> fileString;

					if (stoi(fileString) == -1) {
						goBack = true;
					}
					else {
						filePath = fileString.c_str();
						cout << "-------------------------------------" << endl;
						cout << "RUNNING: " << fileString << endl;
						cout << "-------------------------------------" << endl;
						doStuff();
					}
					break;


				case 4:
					quit = true;
					break;

				default:
					cout << "Bad input, try again" << endl << endl;
					break;
				}
		}

	}
	return 0;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// IF YOU WANT TO OVERRIDE THE MAIN MENU OPTIONS AND DIRECTLY HARD-CODE IN A FILEPATH OF SOURCE TO READ, UNCOMMENT BELOW AND ENSURE THE FILEPATH IS CORRECT // 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//const char* filePath = "..//testPgms//correct//iterativeFib.src"; // Filepath to read
}
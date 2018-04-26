#pragma once

#include <vector>

using namespace std;

// Structure to handle parsing errors
struct ParsingError {
	string parseError;
	int lineNumber;
	string tokenName;

	ParsingError(string errorString, int LineNum, string tokName) : parseError(errorString), lineNumber(LineNum), tokenName(tokName) {}
};

// Structure to handle lexer errors
struct ScannerError {
	string scanError;
	int lineNumber;
	string tokenName;

	ScannerError(string errorString, int LineNum, string tokName) : scanError(errorString), lineNumber(LineNum), tokenName(tokName) {}
};

// Structure to handle symbol table errors
struct SymbolError {
	string symbolError;
	string idString;

	SymbolError(string errorString, string id) : symbolError(errorString), idString(id) {}
};

// Make vectors of each error type available to the other code files
extern vector<ParsingError> ResultOfParse;
extern vector<ScannerError> ResultOfScan;
extern vector<SymbolError> ResultOfSymbol;
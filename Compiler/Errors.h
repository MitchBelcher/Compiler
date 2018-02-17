#pragma once

#include <vector>

using namespace std;

struct ParsingError {
	string parseError;
	int lineNumber;
	string tokenName;

	ParsingError(string errorString, int LineNum, string tokName) : parseError(errorString), lineNumber(LineNum), tokenName(tokName) {}
};

struct ScannerError {
	string scanError;
	int lineNumber;
	string tokenName;

	ScannerError(string errorString, int LineNum, string tokName) : scanError(errorString), lineNumber(LineNum), tokenName(tokName) {}
};

struct SymbolError {
	string symbolError;
	string idString;

	SymbolError(string errorString, string id) : symbolError(errorString), idString(id) {}
};

extern vector<ParsingError> ResultOfParse;
extern vector<ScannerError> ResultOfScan;
extern vector<SymbolError> ResultOfSymbol;
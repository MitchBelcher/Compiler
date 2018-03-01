/*

This header file contains the base Scanner class that holds a constructor/destructor pair,
as well as the function declarations for scanning in tokens, a vector to contain the tokens for easy access,
A function to check if a given string is a reserve word, and a temporary filestream input.

*/

#pragma once

#include "Token.h"
#include "SymTable.h"

#include <vector>

class Scanner {
public:
	void init (const char* filePath, SymTable& returnedSymbolTable); // Constructor (pass filepath)
	~Scanner(); // Destructor

	//void scanIn (const char* filePath);
	token tokenScan (); // Scan in a token
	vector<token> getTokens(); // Vector to get tokens out

private:
	//token tokenScan (FILE* stream);
	vector<token> tempTokenList; // Temporary token vector
	FILE* tempStream; // Temporary input filestream
	SymTable* symbolTable;
};
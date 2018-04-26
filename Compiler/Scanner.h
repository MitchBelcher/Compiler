/*

This header file contains the base Scanner class that holds a constructor/destructor pair,
as well as the function declarations for scanning in tokens, a vector to contain the tokens for easy access,
A function to check if a given string is a reserve word, and a temporary filestream input.

*/

#pragma once

#include "Token.h"
#include "SymTable.h"

#include <vector>

// Scanner class
class Scanner {
public:
	void init (const char* filePath, SymTable& returnedSymbolTable);	// Constructor
	~Scanner();															// Destructor

	token tokenScan (); // Function to scan a token in

private:
	FILE* tempStream;		// Input file stream
	SymTable* symbolTable;	// Symbol table instance
};
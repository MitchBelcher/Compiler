/*

This header file contains the base Symbol Table class, and creates a hash table map

*/

#pragma once

#include <stack>
#include <string>
#include <unordered_map>

#include "Token.h"

using namespace std;

typedef unordered_map<string, Symbol> HashTable; // Define a hash table (unordered map) made of strings and Symbol structures

// Symbol table class
class SymTable {
public:
	SymTable(); // Constructor

	Symbol* getSymbol(string id, bool onlyGlobal = false);			// Function to get a symbol
	Symbol* addSymbol(string id, Symbol symbolIn, bool isGlobal);	// Function to add a symbol

	void OpenScope();	// Function to open a scope
	void CloseScope();	// Function to close a scope

private:
	HashTable GlobalTable;		// Global symbol table
	vector<HashTable> Scopes;	// Vector of local scope symbol tables (furthest occuppied space is always the current scope)
};
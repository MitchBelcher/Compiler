/*

This header file contains the base Symbol Table class, and creates a hash table map

*/

#pragma once

#include <stack>
#include <string>
#include <unordered_map>

#include "Token.h"

using namespace std;

typedef unordered_map<string, Symbol> HashTable;

class SymTable
{
public:
	SymTable();

	Symbol* getSymbol(string id, bool onlyGlobal = false);
	Symbol* addSymbol(string id, Symbol symbolIn, bool isGlobal);

	void OpenScope();
	void CloseScope();

private:
	HashTable GlobalTable;
	vector<HashTable> Scopes;
};
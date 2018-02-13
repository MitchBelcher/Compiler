/*

This header file contains the base Symbol Table class, and creates a hash table map

*/

#pragma once

#include <stack>
#include <string>
#include <unordered_map>

#include "Token.h"


typedef std::unordered_map<std::string, Symbol> HashTable;

class SymTable
{
public:

	Symbol * getSymbol(std::string idString);
	void addSymbol(std::string idString, Symbol symbolToAdd, bool isGlobal);

	void OpenScope();
	void CloseScope();

private:
	HashTable GlobalSymbolTable;
	std::stack<HashTable> SymbolTableList;
};
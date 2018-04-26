#include "SymTable.h"
#include "Errors.h"
#include <iostream>

using namespace std;


// Constructor to initialize the symbol table
// Adds all of the built in functions and reserve words as symbols to the global table
SymTable::SymTable() {
	Scopes.reserve(100);
	Symbol reserveSymbol;
	reserveSymbol.isGlobal = true;

	reserveSymbol.id = "program";
	reserveSymbol.tempTokenType = PROGRAM;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "begin";
	reserveSymbol.tempTokenType = BEGIN;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "end";
	reserveSymbol.tempTokenType = END;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "global";
	reserveSymbol.tempTokenType = GLOBAL;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "procedure";
	reserveSymbol.tempTokenType = PROCEDURE;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "is";
	reserveSymbol.tempTokenType = IS;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "in";
	reserveSymbol.tempTokenType = IN;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "out";
	reserveSymbol.tempTokenType = OUT;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "inout";
	reserveSymbol.tempTokenType = INOUT;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "not";
	reserveSymbol.tempTokenType = NOT;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "if";
	reserveSymbol.tempTokenType = IF;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "then";
	reserveSymbol.tempTokenType = THEN;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "else";
	reserveSymbol.tempTokenType = ELSE;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "for";
	reserveSymbol.tempTokenType = FOR;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "return";
	reserveSymbol.tempTokenType = RETURN;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "string";
	reserveSymbol.tempTokenType = STRING;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "bool";
	reserveSymbol.tempTokenType = BOOL;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "char";
	reserveSymbol.tempTokenType = CHAR;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "float";
	reserveSymbol.tempTokenType = FLOAT;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "integer";
	reserveSymbol.tempTokenType = INTEGER;
	addSymbol(reserveSymbol.id, reserveSymbol, true);




	reserveSymbol.tempSymbolType = PROC;
	reserveSymbol.tempTokenType = IDENTIFIER;
	Symbol varAdd;
	Symbol symbolAdd;
	Symbol* retVarSymbol;
	Symbol* retSymbol;
	varAdd.id = "val";
	varAdd.tempTokenType = IDENTIFIER;
	varAdd.isGlobal = false;

	reserveSymbol.id = "putinteger";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMINTEGER;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, IN));
	CloseScope();

	reserveSymbol.id = "putfloat";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMFLOAT;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, IN));
	CloseScope();

	reserveSymbol.id = "putchar";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMCHAR;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, IN));
	CloseScope();

	reserveSymbol.id = "putbool";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMBOOL;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, IN));
	CloseScope();

	reserveSymbol.id = "putstring";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMSTRING;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, IN));
	CloseScope();

	reserveSymbol.id = "getinteger";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMINTEGER;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, OUT));
	CloseScope();

	reserveSymbol.id = "getfloat";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMFLOAT;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, OUT));
	CloseScope();

	reserveSymbol.id = "getchar";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMCHAR;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, OUT));
	CloseScope();

	reserveSymbol.id = "getbool";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMBOOL;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, OUT));
	CloseScope();

	reserveSymbol.id = "getstring";
	retSymbol = addSymbol(reserveSymbol.id, reserveSymbol, true);
	OpenScope();
	varAdd.tempSymbolType = SYMSTRING;
	retVarSymbol = addSymbol(varAdd.id, varAdd, false);
	retSymbol->procedureParameters.push_back(pair<Symbol*, TYPES>(retVarSymbol, OUT));
	CloseScope();







	reserveSymbol.id = "true";
	reserveSymbol.tempTokenType = TRUE;
	reserveSymbol.tempSymbolType = SYMBOOL;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "false";
	reserveSymbol.tempTokenType = FALSE;
	reserveSymbol.tempSymbolType = SYMBOOL;
	addSymbol(reserveSymbol.id, reserveSymbol, true);
}


// Function to get a symbol given whether or not it's global and it's ID
Symbol* SymTable::getSymbol(string id, bool onlyGlobal) {

	Symbol* symbolOut = nullptr;

	// If we are not searching in global, and we have opened a scope, look for the symbol in there
	if (!Scopes.empty() && onlyGlobal == false) {
		auto iter = Scopes[Scopes.size() - 1].find(id);
		if (iter != Scopes[Scopes.size() - 1].end()) {
			symbolOut = &(iter->second);
		}
	}

	// We are searching for a global symbol, check the global table for it
	if (symbolOut == nullptr) {
		auto iter = GlobalTable.find(id);
		if (iter != GlobalTable.end()) {
			symbolOut = &(iter->second);
		}
	}
	return symbolOut;
}


// Function to add a symbol given the symbol, it's ID, and whether or not it's global
Symbol* SymTable::addSymbol(string id, Symbol symbolIn, bool isGlobal) {

	// If the symbol is supposed to be global, insert it there
	if (isGlobal) {
		GlobalTable.insert({ id, symbolIn });

		auto iter = GlobalTable.find(id);
		return &(iter->second);
	}

	// Symbol is not meant to be global
	else {

		// Check if we've opened a scope, if we have, add the symbol to the newest scope
		if (!Scopes.empty()) {
			Scopes[Scopes.size() - 1].insert({ id, symbolIn });

			auto iter = Scopes[Scopes.size() - 1].find(id);
			return &(iter->second);
		}

		// We've not entered a procedure yet, add the symbol to the global table instead
		else {
			symbolIn.isGlobal = true;
			GlobalTable.insert({ id, symbolIn });

			auto iter = GlobalTable.find(id);
			return &(iter->second);

			SymbolError tempError("WARNING, ASSUMED VALUE WAS MEANT TO BE ADDED TO GLOBAL", symbolIn.id);
			ResultOfSymbol.push_back(tempError);
		}
	}
	return nullptr;
}


// Function to open a new scope
void SymTable::OpenScope() {
	Scopes.push_back(HashTable());
}


// Function to close the current scope
void SymTable::CloseScope() {
	Scopes.pop_back();
}
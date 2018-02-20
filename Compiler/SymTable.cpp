#include "SymTable.h"
#include "Errors.h"

using namespace std;

SymTable::SymTable() {
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

	reserveSymbol.id = "true";
	reserveSymbol.tempTokenType = TRUE;
	addSymbol(reserveSymbol.id, reserveSymbol, true);

	reserveSymbol.id = "false";
	reserveSymbol.tempTokenType = FALSE;
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

	

	/*
	
	PROGRAM,
	BEGIN, END,
	GLOBAL,
	PROCEDURE,
	PARENBEGIN, PARENEND,
	IS,
	SEMICOLON, COMMA, COLON,
	IN, OUT, INOUT,
	BRACKBEGIN, BRACKEND,
	AND, OR, NOT,
	IF, THEN, ELSE, FOR,
	SEMIEQUAL,
	ADD, SUB, MULT, DIVIDE,
	GREAT, LESS, GREATEQ, LESSEQ,
	EQUALS, NOTEQUAL, DOUBLEEQUAL,
	TRUE, FALSE,
	RETURN,
	IDENTIFIER,
	STRING, VALSTRING,
	BOOL, VALBOOL,
	CHAR, VALCHAR,
	FLOAT, VALFLOAT,
	INTEGER, VALINT,
	INVALID,
	COMMENT,
	FILEEND, STREAMEND

	*/
}

Symbol* SymTable::getSymbol(string id, bool onlyGlobal) {

	Symbol* symbolOut = nullptr;

	if (!Scopes.empty() && onlyGlobal == false) {
		auto iter = Scopes[Scopes.size() - 1].find(id);
		if (iter != Scopes[Scopes.size() - 1].end()) {
			symbolOut = &(iter->second);
		}
	}

	if (symbolOut == nullptr) {
		auto iter = GlobalTable.find(id);
		if (iter != GlobalTable.end()) {
			symbolOut = &(iter->second);
		}
	}
	
	return symbolOut;
}


Symbol* SymTable::addSymbol(string id, Symbol symbolIn, bool isGlobal) {

	if (isGlobal) {
		GlobalTable.insert({ id, symbolIn });

		auto iter = GlobalTable.find(id);
		if (iter != GlobalTable.end()) {
			return &(iter->second);
		}
		else {
			// ERROR, UNABLE TO ADD TO GLOBAL TABLE
			SymbolError tempError("ERROR, UNABLE TO ADD SYMBOL TO GLOBAL TABLE", symbolIn.id);
			ResultOfSymbol.push_back(tempError);
		}
	}
	else {
		if (!Scopes.empty()) {
			Scopes[Scopes.size() - 1].insert({ id, symbolIn });

			auto iter = Scopes[Scopes.size() - 1].find(id);
			if (iter != Scopes[Scopes.size() - 1].end()) {
				return &(iter->second);
			}
			else {
				// ERROR, UNABLE TO ADD TO LOCAL SCOPE
				SymbolError tempError("ERROR, UNABLE TO ADD SYMBOL TO LOCAL SCOPE", symbolIn.id);
				ResultOfSymbol.push_back(tempError);
			}
		}
		else {
			GlobalTable.insert({ id, symbolIn });
			// THROW WARNING, ADDED TO GLOBAL BY DEFAULT SINCE NO OPEN SCOPE(S)
			SymbolError tempError("WARNING, ASSUMED VALUE WAS MEANT TO BE ADDED TO GLOBAL", symbolIn.id);
			ResultOfSymbol.push_back(tempError);
		}
	}
	return nullptr;
}

void SymTable::OpenScope() {
	Scopes.push_back(HashTable());
}

void SymTable::CloseScope() {
	Scopes.pop_back();
}
/*

This header file contains the base Parser class that holds a constructor, a function to parse through the file,
An instance of Scanner, a temporary token for parsing, as well as declarations for all parsing function steps

*/

#pragma once

#include "Scanner.h"

#include <vector>

struct DataStore {
	token tempToken;
	SYMBOL_TYPES tempType;
	vector<Symbol*> procedureParamters;
};

class Parser {
public:
	Parser(const char* filePath, SymTable& newSymbolTable);
	void parseFile();

	Scanner inputScanner;

private:
	token tempToken;
	SymTable* symbolTable;

	DataStore Program();
	DataStore ProgramHead();
	DataStore ProgramBody();
	DataStore Declare();
	DataStore Statement();
	DataStore ProcDeclare(bool isGlobal);
	DataStore VarDeclare(bool isGlobal);
	DataStore ProcHead(bool isGlobal);
	DataStore ProcBody();
	DataStore ParamList();
	DataStore Param();
	DataStore TypeMark();
	DataStore Assign();
	DataStore If();
	DataStore Loop();
	DataStore Return();
	DataStore AssignState();
	DataStore ArgumentList();

	DataStore Expr();
	DataStore ExprPrime();
	DataStore Arith();
	DataStore ArithPrime();
	DataStore Relat();
	DataStore RelatPrime();
	DataStore Term();
	DataStore TermPrime();
	DataStore Factor();
	DataStore Name();
	DataStore Number();
	DataStore Ident();
};
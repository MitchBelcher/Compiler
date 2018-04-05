/*

This header file contains the base Parser class that holds a constructor, a function to parse through the file,
An instance of Scanner, a temporary token for parsing, as well as declarations for all parsing function steps

*/

#pragma once

#include "Scanner.h"
#include "CodeGen.h"

#include <vector>

struct DataStore {
	bool success = true;
	token tempToken;
	SYMBOL_TYPES tempType;
	vector<pair<Symbol*, PARAM_TYPES>> procedureParameters = {};
	vector<SYMBOL_TYPES> args = {};
};

class Parser {
public:
	Parser(const char* filePath, SymTable& returnedSymbolTable);
	void parseFile();

	Scanner inputScanner;

private:
	token tempToken;
	SymTable* symbolTable;
	CodeGen codeGenerator;

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
	DataStore Assign(bool onlyAssign);
	DataStore If();
	DataStore Loop();
	DataStore Return();
	DataStore AssignState(token destTok, SYMBOL_TYPES destType);
	DataStore ArgumentList();

	DataStore Expr();
	DataStore ExprPrime(token prevFacTok, SYMBOL_TYPES prevFacType);
	DataStore Arith();
	DataStore ArithPrime(token prevFacTok, SYMBOL_TYPES prevFacType);
	DataStore Relat();
	DataStore RelatPrime(token prevFacTok, SYMBOL_TYPES prevFacType);
	DataStore Term();
	DataStore TermPrime(token prevFacTok, SYMBOL_TYPES prevFacType);
	DataStore Factor();
	DataStore Name();
	DataStore Number();
	DataStore Ident();
};
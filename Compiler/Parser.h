/*

This header file contains the base Parser class that holds a constructor, a function to parse through the file,
An instance of Scanner, a temporary token for parsing, as well as declarations for all parsing function steps

*/

#pragma once

#include "Scanner.h"
#include "CodeGen.h"
#include "SymTable.h"

#include <vector>


// Structure for returning data up out of the parsing tree that includes a success flag, the token, the symbol type, the potential parameters, and the potential arguments
struct DataStore {
	bool success = true;
	token tempToken;
	SYMBOL_TYPES tempType;
	vector<tuple<Symbol*, TYPES>> procedureParameters;
	vector<SYMBOL_TYPES> args;
};


// Parser class
class Parser {
public:
	Parser(const char* filePath, SymTable& returnedSymbolTable);	// Constructor

	void beginParsingFile();	// Function to start the parse

	Scanner inputScanner;	// Instance of Scanner

private:
	token tempToken;		// Instance of a token structure
	SymTable* symbolTable;	// Instance of a symbol table
	CodeGen codeGenerator;	// Instance of a code generator

	DataStore Program();
	DataStore ProgramHead();
	DataStore ProgramBody();
	DataStore Declare(bool isOnlyGlobal);
	DataStore Statement(bool acceptParen);
	DataStore ProcDeclare(bool isGlobal);
	DataStore VarDeclare(bool isGlobal);
	DataStore ProcHead(bool isGlobal);
	DataStore ProcBody();
	DataStore ParamList();
	DataStore Param();
	DataStore Type();
	DataStore Assign();
	DataStore ParenAssign();
	DataStore If();
	DataStore Loop();
	DataStore Return();
	DataStore SetAssign(token destTok, SYMBOL_TYPES destType);
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
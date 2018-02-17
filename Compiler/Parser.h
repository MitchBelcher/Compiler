/*

This header file contains the base Parser class that holds a constructor, a function to parse through the file,
An instance of Scanner, a temporary token for parsing, as well as declarations for all parsing function steps

*/

#pragma once

#include "Scanner.h"

struct symInfo {
	bool isCorrect = true;
	SYMBOL_TYPES p_type;
	token p_token;
};

class Parser {
public:
	Parser(const char* filePath, SymTable& newSymbolTable);
	void parseFile();

	Scanner inputScanner;

private:
	token tempToken;
	SymTable* symbolTable;

	void Program();
	void ProgramHead();
	void ProgramBody();
	void Declare();
	void Statement();
	void ProcDeclare();
	void VarDeclare();
	void ProcHead();
	void ProcBody();
	void ParamList();
	void Param();
	void TypeMark();
	void Assign();
	void If();
	void Loop();
	void Return();
	void AssignState();
	void ArgumentList();

	void Expr();
	void ExprPrime();
	void Arith();
	void ArithPrime();
	void Relat();
	void RelatPrime();
	void Term();
	void TermPrime();
	void Factor();
	void Name();
	void Number();
	void Ident();
};
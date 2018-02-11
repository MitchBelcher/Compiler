/*

This header file contains the base Parser class that holds a constructor, a function to parse through the file,
An instance of Scanner, a temporary token for parsing, as well as declarations for all parsing function steps

*/

#pragma once

#include "Scanner.h"
#include <vector>

struct ParsingError {
	string errorString;
	int lineNumber;

	ParsingError(string string, int LineNum) : errorString(string), lineNumber(LineNum) {}
};

class Parser {
public:
	Parser(const char* filePath);
	void parseFile();

	Scanner inputScanner;
	vector<ParsingError> ResultOfParse;

private:
	token tempToken;

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
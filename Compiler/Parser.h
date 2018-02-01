#pragma once

#include "Scanner.h"

class Parser {
public:
	Parser(const char* filePath);
	void parseFile();

	Scanner inputScanner;

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
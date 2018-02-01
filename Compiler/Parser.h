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
	void Program_Head();
	void Program_Body();
	void Declar();
	void Statement();
	void Proc_Declar();
	void Var_Declar();
	void Proc_Head();
	void Proc_Body();
	void Param_List();
	void Param();
	void Type_Mark();

	void Expr();
	void Expr_Prime();
	void Arith();
	void Arith_Prime();
	void Relat();
	void Relat_Prime();
	void Term();
	void Term_Prime();
	void Factor();
	void Name();
	void Number();
	void Ident();
};
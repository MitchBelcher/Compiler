/*

This cpp file contains the definition for Parser class functions, each going through the LL1 parse tree for this language

*/

#include "Parser.h"

// Main parser constructor, which calls the constructor for the scanner instance
Parser::Parser(const char* filePath) {
	inputScanner.init(filePath);
}

// Begin parsing the file by scanning the first token, and then running the program procedures
void Parser::parseFile() {
	tempToken = inputScanner.tokenScan();
	Program();
}

// Program
void Parser::Program() {

	// If the first token is the program reserve word, we can proceed
	if (tempToken.t_type == PROGRAM) {
		ProgramHead(); // Run program header procedure
		ProgramBody(); // Rune program body procedure
	}
	else {
		// ERROR FIRST TOKEN WAS NOT PROGRAM RESERVE WORD, VIOLATION OF LANGUAGE
	}
}

// Program Head
void Parser::ProgramHead() {

	// Check that the token is the program reserve word, ADDED PROGRAM TO TREE
	if (tempToken.t_type == PROGRAM) {
		tempToken = inputScanner.tokenScan(); // Get next token
		Ident(); // Run identifier procedure

		tempToken = inputScanner.tokenScan(); // Get next token

		// Check that the token is the is reserve word
		if (tempToken.t_type == IS) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED IS TO TREE
		}

		else {
			// ERROR
		}
	}
}

// Program Body
void Parser::ProgramBody() {

	// CHECK FOR DECLARATION
	// If the token has type of global, procedure, integer, float, bool, string or char
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare(); // Run declaration procedure

		tempToken = inputScanner.tokenScan(); // Get next token

		// Check that the token is the semicolon type
		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
		}

		else {
			// ERROR
		}
	}

	// CHECK FOR BEGIN
	// Check that the token is the begin reserve word
	if (tempToken.t_type == BEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED BEGIN TO TREE
	}
	else {
		// ERROR, BEGIN RESERVE WORD NOT FOUND, VIOLATION OF LANGUAGE
	}

	// CHECK FOR STATEMENT
	// If the token has type of identifier, if, for, or return
	while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
		Statement(); // Run statement procedure

		// Check that the token is the semicolon type
		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
		}
		else {
			// ERROR
		}
	}
}


void Parser::Declare() {

}


void Parser::Statement() {

}


void Parser::ProcDeclare() {

}


void Parser::VarDeclare() {

}


void Parser::ProcHead() {

}


void Parser::ProcBody() {

}


void Parser::ParamList() {

}


void Parser::Param() {

}


void Parser::TypeMark() {

}


void Parser::Assign() {

}


void Parser::If() {

}


void Parser::Loop() {

}


void Parser::Return() {

}


void Parser::ProcCall() {

}














void Parser::Ident() {

}


void Parser::Expr() {

}


void Parser::ExprPrime() {

}


void Parser::Arith() {

}


void Parser::ArithPrime() {

}


void Parser::Relat() {

}


void Parser::RelatPrime() {

}


void Parser::Term() {

}


void Parser::TermPrime() {

}


void Parser::Factor() {

}


void Parser::Name() {

}


void Parser::Number() {

}
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

// Declare
void Parser::Declare() {

	// CHECK FOR GLOBAL
	if (tempToken.t_type == GLOBAL) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED GLOBAL TO TREE

		// CHECK FOR PROCEDURE
		if (tempToken.t_type == PROCEDURE) {
			ProcDeclare(); // Run procedure declaration procedure
		}

		// Check if type is a variable
		else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
			VarDeclare(); // Run variable declaration procedure
		}
	}

	// CHECK FOR PROCEDURE
	else if (tempToken.t_type == PROCEDURE) {
		ProcDeclare(); // Run procedure declaration procedure
	}

	// Check if type is a variable
	else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
		VarDeclare(); // Run variable declaration procedure
	}
}

// Statement
void Parser::Statement() {

	// Assign statement/Procedure 
	if (tempToken.t_type == IDENTIFIER) {
		/////////////////////////////////
		// TODO /////////////////////////
		/////////////////////////////////
	}

	// If conditional start
	else if (tempToken.t_type == IF) {
		If(); // Run if procedure
	}

	// For loop start
	else if (tempToken.t_type == FOR) {
		Loop(); // Run loop procedure
	}

	// Return statement
	else if (tempToken.t_type == RETURN) {
		Return(); // Run return procedure
	}
	else {
		// ERROR, NO VALID STATEMENT PRESENT
	}
}

// Procedure Declaration
void Parser::ProcDeclare() {
	ProcHead(); // Run procedure head procedure
	ProcBody(); // Run procedure body procedure
}

// Variable Declaration
void Parser::VarDeclare() {
	TypeMark();
	Ident();

	// Check for left bracket
	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADD LEFT BRACKET TO TREE
		Number(); // Run number procedure

		// Check for colon
		if (tempToken.t_type == COLON) {
			tempToken = inputScanner.tokenScan(); // Get next token
			Number();

			// Check for right bracket
			if (tempToken.t_type == BRACKEND) {

			}
		}
	}
}

// Procedure Head
void Parser::ProcHead() {

}

// Procedure Body
void Parser::ProcBody() {

}

// Parameter List
void Parser::ParamList() {

}

// Parameter
void Parser::Param() {

}

// Type Mark
void Parser::TypeMark() {

}

// Assign
void Parser::Assign() {

}

// If
void Parser::If() {

}

// Loop
void Parser::Loop() {

}

// Return
void Parser::Return() {

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
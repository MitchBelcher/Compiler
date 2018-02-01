#include "Parser.h"

Parser::Parser(const char* filePath) {
	inputScanner.init(filePath);
}

void Parser::parseFile() {
	tempToken = inputScanner.tokenScan();
	Program();
}

void Parser::Program() {
	if (tempToken.t_type == PROGRAM) {
		ProgramHead();
		ProgramBody();
	}
	else {
		// ERROR
	}
}

void Parser::ProgramBody() {
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare();

		tempToken = inputScanner.tokenScan();

		if (tempToken.t_type == SEMICOLON) {

			// ADD TO TREE

			tempToken = inputScanner.tokenScan();
		}

		else {
			// ERROR
		}
	}
}
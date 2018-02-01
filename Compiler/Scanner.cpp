#include "Scanner.h"

#include <string>
#include <stdio.h>

vector<token> Scanner::getTokens() {
	return tempTokenList;
}

void Scanner::checkForReserves(token &tempToken) {

		if (tempToken.t_string.compare("program") == 0) {
			tempToken.t_type = PROGRAM;
		}

		if (tempToken.t_string.compare("begin") == 0) {
			tempToken.t_type = BEGIN;
		}

		if (tempToken.t_string.compare("end") == 0) {
			tempToken.t_type = END;
		}

		if (tempToken.t_string.compare("global") == 0) {
			tempToken.t_type = GLOBAL;
		}
		
		if (tempToken.t_string.compare("procedure") == 0) {
			tempToken.t_type = PROCEDURE;
		}

		if (tempToken.t_string.compare("in") == 0) {
			tempToken.t_type = IN;
		}

		if (tempToken.t_string.compare("out") == 0) {
			tempToken.t_type = OUT;
		}

		if (tempToken.t_string.compare("inout") == 0) {
			tempToken.t_type = INOUT;
		}

		if (tempToken.t_string.compare("float") == 0) {
			tempToken.t_type = FLOAT;
		}

		if (tempToken.t_string.compare("bool") == 0) {
			tempToken.t_type = BOOL;
		}

		if (tempToken.t_string.compare("char") == 0) {
			tempToken.t_type = CHAR;
		}

		if (tempToken.t_string.compare("if") == 0) {
			tempToken.t_type = IF;
		}

		if (tempToken.t_string.compare("then") == 0) {
			tempToken.t_type = THEN;
		}

		if (tempToken.t_string.compare("else") == 0) {
			tempToken.t_type = ELSE;
		}

		if (tempToken.t_string.compare("for") == 0) {
			tempToken.t_type = FOR;
		}

		if (tempToken.t_string.compare("return") == 0) {
			tempToken.t_type = RETURN;
		}

		if (tempToken.t_string.compare("string") == 0) {
			tempToken.t_type = STRING;
		}

		if (tempToken.t_string.compare("integer") == 0) {
			tempToken.t_type = INTEGER;
		}
}

token Scanner::tokenScan(FILE* stream) {

	token tempToken;
	int currentChar;

	currentChar = getc(stream);

	// Check for whitespace
	while (isspace(currentChar)) {
		currentChar = getc(stream);
	}

	// Parentheses begin
	if (currentChar == '(') {
		tempToken.t_type = PARENBEGIN;
		tempToken.t_char = currentChar;

	}

	// Parentheses end
	else if (currentChar == ')') {
		tempToken.t_type = PARENEND;
		tempToken.t_char = currentChar;
	}

	// File end
	else if (currentChar == '.') {
		tempToken.t_type = FILEEND;
		tempToken.t_char = currentChar;
	}

	// Identifiers, reserve words or true/false
	else if (isalpha(currentChar)) {

		tempToken.t_type = IDENTIFIER;
		tempToken.t_string = tolower(currentChar);

		int nextChar; // create a variable for next character
		nextChar = getc(stream); // Get the next character

		// If the next character is alpha, digit or _, continue grabbing the next character and append it to the string
		while (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_') {
			tempToken.t_string += tolower(nextChar);
			nextChar = getc(stream);
		}
		ungetc(nextChar, stream); // Put invalid character back on the stream

		checkForReserves(tempToken);
	}

	// Chars
	else if (currentChar == 39) {

		int nextChar; // create a variable for next character
		nextChar = getc(stream); // Get the next character

		if (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_' || nextChar == ';' || nextChar == ':' || nextChar == '.' || nextChar == '"' || nextChar == ' ') {
			tempToken.t_char = nextChar;
			nextChar = getc(stream);
		}

		if (nextChar == 39) {
			tempToken.t_type = VALCHAR;
		}

		else {
			tempToken.t_type = INVALID;
		}
	}

	// Strings
	else if (currentChar == '"') {

		tempToken.t_string += currentChar; // set up token string

		int nextChar; // create a variable for next character
		nextChar = getc(stream); // Get the next character

		// Continue grabbing the next character and append it to the string
		while (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_' || nextChar == ';' || nextChar == ':' || nextChar == '.' || nextChar == ',' || nextChar == 39 || nextChar == ' ') {
			tempToken.t_string += nextChar;
			nextChar = getc(stream);
		}

		if (nextChar == '"') {
			tempToken.t_type = VALSTRING;
		}
		else {
			tempToken.t_type = INVALID;
		}

	}

	// Numbers (int or float)
	else if (isdigit(currentChar)) {

		tempToken.t_string += currentChar; // set up token string

		int nextChar; // create a variable for next character
		nextChar = getc(stream); // Get the next character

		while (isdigit(nextChar) || nextChar == '_') {

			if (nextChar != '_') {
				tempToken.t_string += nextChar;
			}

			nextChar = getc(stream);
		}

		if (nextChar == '.') {

			tempToken.t_string += nextChar;

			nextChar = getc(stream);

			while (isdigit(nextChar) || nextChar == '_') {

				if (nextChar != '_') {
					tempToken.t_string += nextChar;
				}

				nextChar = getc(stream);
			}

			ungetc(nextChar, stream);
			tempToken.t_type = VALFLOAT;
			tempToken.t_float = atof(tempToken.t_string.c_str());
		}

		else {
			ungetc(nextChar, stream);
			tempToken.t_type = VALINT;
			tempToken.t_int = atoi(tempToken.t_string.c_str());
		}

	}

	// And
	else if (currentChar == '&') {
		tempToken.t_type = AND;
		tempToken.t_char = currentChar;
	}

	// Or
	else if (currentChar == '|') {
		tempToken.t_type = OR;
		tempToken.t_char = currentChar;
	}

	// ;
	else if (currentChar == ';') {
		tempToken.t_type = SEMICOLON;
		tempToken.t_char = currentChar;
	}

	// ,
	else if (currentChar == ',') {
		tempToken.t_type = COMMA;
		tempToken.t_char = currentChar;
	}

	// : or :=
	else if (currentChar == ':') {

		tempToken.t_string += currentChar;

		int nextChar;

		nextChar = getc(stream);

		if (nextChar = '=') {
			tempToken.t_type = SEMIEQUAL;
		}

		else {
			tempToken.t_type = COLON;
			ungetc(nextChar, stream);
		}
	}

	// [
	else if (currentChar == '[') {
		tempToken.t_type = BRACKBEGIN;
		tempToken.t_char = currentChar;
	}

	// ]
	else if (currentChar == ']') {
		tempToken.t_type = BRACKEND;
		tempToken.t_char = currentChar;
	}

	// = or ==
	else if (currentChar == '=') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(stream);

		if (nextChar == '=') {
			tempToken.t_type = DOUBLEEQUAL;
		}
		else {
			tempToken.t_type = EQUALS;
			ungetc(nextChar, stream);
		}
	}

	// < or <=
	else if (currentChar == '<') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(stream);

		if (nextChar == '=') {
			tempToken.t_type = LESSEQ;
		}
		else {
			tempToken.t_type = LESS;
			ungetc(nextChar, stream);
		}
	}

	// > or >=
	else if (currentChar == '>') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(stream);

		if (nextChar == '=') {
			tempToken.t_type = GREATEQ;
		}
		else {
			tempToken.t_type = GREAT;
			ungetc(nextChar, stream);
		}
	}

	// !=
	else if (currentChar == '!') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(stream);

		if (nextChar == '=') {
			tempToken.t_type = NOTEQUAL;
		}
		else {
			tempToken.t_type = INVALID;
			ungetc(nextChar, stream);
		}
	}

	// +
	else if (currentChar == '+') {
		tempToken.t_type = ADD;
		tempToken.t_char = currentChar;
	}

	// -
	else if (currentChar == '-') {
		tempToken.t_type = SUB;
		tempToken.t_char = currentChar;
	}

	// * or */
	else if (currentChar == '*') {
		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(stream);

		if (nextChar == '/') {
			//////////////////////////////////////////
			// TODO //////////////////////////////////
			//////////////////////////////////////////
			tempToken.t_type = EDBLKCOMMENT;
		}
		else {
			tempToken.t_type = MULT;
			tempToken.t_char = currentChar;
		}
	}

	// /, // or /*
	else if (currentChar == '/') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(stream);

		if (nextChar == '/') {

			while (nextChar != '\n') {
				tempToken.t_string += nextChar;
				nextChar = getc(stream);
			}

			ungetc(nextChar, stream);

			tempToken.t_type = COMMENT;
		}

		else if (nextChar == '*') {
			///////////////////////////////////
			// TODO ///////////////////////////
			///////////////////////////////////
			tempToken.t_type = STBLKCOMMENT;
		}

		else {
			tempToken.t_type = DIVIDE;
			tempToken.t_char = currentChar;
			ungetc(nextChar, stream);
		}
	}

	// Invalid
	else {
		tempToken.t_type = INVALID;
		tempToken.t_char = currentChar;
	}

	return tempToken;

}

void Scanner::scanIn(const char* filePath) {

	errno_t error = fopen_s(&tempStream, filePath, "r"); // Open read-only filestream with specified file path

	 // Error in open
	if (error != 0) {
		// File cannot be read
	}

	token firstToken = tokenScan(tempStream);

	while (firstToken.t_type != FILEEND) {
		tempTokenList.push_back(firstToken);
		firstToken = tokenScan(tempStream);
	}
	tempTokenList.push_back(firstToken);

	fclose(tempStream);
}
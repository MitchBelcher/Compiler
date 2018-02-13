/*

This cpp file contains the definitions for Scanner class functions, like initialization/construction and scanning to create tokens

*/

#include "Scanner.h"

#include <string>
#include <stdio.h>

int currentLineNumber = 1;
int commentDepth = 0;

// Simple function to return a list of tokens
vector<token> Scanner::getTokens() {
	return tempTokenList;
}

// Function to check if input reference token is one of a number of reserve words versus a simple identifier
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

		if (tempToken.t_string.compare("true") == 0) {
			tempToken.t_type = TRUE;
			tempToken.t_bool = true;
		}

		if (tempToken.t_string.compare("false") == 0) {
			tempToken.t_type = FALSE;
			tempToken.t_bool = false;
		}

		if (tempToken.t_string.compare("is") == 0) {
			tempToken.t_type = IS;
		}
}

// Function to systematically scan characters to create a final token
token Scanner::tokenScan() {

	token tempToken; // Create temporary token for scanning

	int currentChar; // Create a placeholder for the current character
	currentChar = getc(tempStream); // Get the current character from the stream

	

	// Check for whitespace
	while (isspace(currentChar)) {

		// Newline
		if (currentChar == '\n') {
			currentLineNumber++; // Increment counter for line number
		}
		currentChar = getc(tempStream); // If whitespace, move to next character
	}

	// Parentheses begin
	if (currentChar == '(') {
		tempToken.t_type = PARENBEGIN;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// Parentheses end
	else if (currentChar == ')') {
		tempToken.t_type = PARENEND;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// File end
	else if (currentChar == '.') {
		tempToken.t_type = FILEEND;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// Identifiers, reserve words or true/false
	else if (isalpha(currentChar)) {

		tempToken.t_type = IDENTIFIER; // Assume the token is a identifier
		tempToken.t_string = tolower(currentChar); // Add the current token to the string (lowercase)
		tempToken.lineNum = currentLineNumber;

		int nextChar; // Create a placeholder for the next character
		nextChar = getc(tempStream); // Get the next character from the stream

		// If the next character is alpha, digit or _, continue grabbing the next character and append it to the string (lowercase)
		while (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_') {
			tempToken.t_string += tolower(nextChar); // Append lowercase to token string
			nextChar = getc(tempStream); // Get the next character from the stream
		}
		ungetc(nextChar, tempStream); // Put invalid character back on the tempStream

		checkForReserves(tempToken); // Check to see if the token was a reserve word, if it wasn't no change is made
	}

	// Characters
	else if (currentChar == 39) {

		int nextChar; // Create a placeholder for the next character
		nextChar = getc(tempStream); // Get the next character from the stream

		// If the next character is any of the allowed characters, add it to the tokens char value, and move to the next character
		if (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_' || nextChar == ';' || nextChar == ':' || nextChar == '.' || nextChar == '"' || nextChar == ' ') {
			tempToken.t_char = nextChar; // Set the tokens char to the character we found
			nextChar = getc(tempStream); // Get the next character from the stream
		}

		// Ensure the next char is a ' to signal the end of the character definition
		if (nextChar == 39) {
			tempToken.t_type = VALCHAR;
			tempToken.lineNum = currentLineNumber;
		}

		// The next char did not signal the end of the character definition, the token violates language rules
		else {
			tempToken.t_type = INVALID;
			ScannerError tempError("ERROR, INVALID CHARACTER DEFINITION", tempToken.lineNum);
			ResultOfScan.push_back(tempError);
		}
	}

	// Strings
	else if (currentChar == '"') {

		//tempToken.t_string += currentChar; // set up token string

		int nextChar; // Create a placeholder for the next character
		nextChar = getc(tempStream); // Get the next character from the stream

		// If the next character is any valid string character, continue grabbing the next character and append it to the string
		while (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_' || nextChar == ';' || nextChar == ':' || nextChar == '.' || nextChar == ',' || nextChar == 39 || nextChar == ' ') {
			tempToken.t_string += nextChar; // Append character to the token string 
			nextChar = getc(tempStream); // Get the next character from the stream
		}

		// Ensure the next char is a " to signal the end of the string definition
		if (nextChar == '"') {
			tempToken.t_type = VALSTRING;
			tempToken.lineNum = currentLineNumber;
		}

		// The next char did not signal the end of the string definition, the token violates language rules
		else {
			tempToken.t_type = INVALID;
			ScannerError tempError("ERROR, INVALID STRING DEFINITION", tempToken.lineNum);
			ResultOfScan.push_back(tempError);
		}

	}

	// Numbers (int or float)
	else if (isdigit(currentChar)) {

		tempToken.t_string += currentChar; // Append first digit char to token string

		int nextChar; // Create a placeholder for the next character
		nextChar = getc(tempStream); // Get the next character from the stream

		// If next char is a digit or underscore, continue grabbing the next character and append it to the string
		while (isdigit(nextChar) || nextChar == '_') {

			// If the char is an underscore, fold it
			if (nextChar != '_') {
				tempToken.t_string += nextChar;
			}

			nextChar = getc(tempStream); // Get the next character from the stream
		}

		// If the next char is a ., start processing it as a float value
		if (nextChar == '.') {

			tempToken.t_string += nextChar; // Append first decimal digit to the token string

			nextChar = getc(tempStream); // Get the next character from the stream

			// If next char is a digit or underscore, continue grabbing the next character and append it to the string
			while (isdigit(nextChar) || nextChar == '_') {

				// If the char is an underscore, fold it
				if (nextChar != '_') {
					tempToken.t_string += nextChar;
				}

				nextChar = getc(tempStream);// Get the next character from the stream
			}

			ungetc(nextChar, tempStream); // Put invalid character back on the temp stream
			tempToken.t_type = VALFLOAT; // Set token type to float, since we found a .
			tempToken.lineNum = currentLineNumber;
			tempToken.t_float = atof(tempToken.t_string.c_str()); // Convert to float, and save in token float element
		}

		// Never found a ., value is an integer, and has ended
		else {
			ungetc(nextChar, tempStream); // Put invalid character back on the temp stream
			tempToken.t_type = VALINT; // Set token type to int, since we never found a .
			tempToken.lineNum = currentLineNumber;
			tempToken.t_int = atoi(tempToken.t_string.c_str()); // Convert to integer, and save in token integer element
		}

	}

	// And
	else if (currentChar == '&') {
		tempToken.t_type = AND;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// Or
	else if (currentChar == '|') {
		tempToken.t_type = OR;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// ;
	else if (currentChar == ';') {
		tempToken.t_type = SEMICOLON;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// ,
	else if (currentChar == ',') {
		tempToken.t_type = COMMA;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// : or :=
	else if (currentChar == ':') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(tempStream);

		tempToken.lineNum = currentLineNumber;

		if (nextChar = '=') {
			tempToken.t_type = SEMIEQUAL;
			tempToken.t_string += nextChar;
		}

		else {
			tempToken.t_type = COLON;
			ungetc(nextChar, tempStream);
		}
	}

	// [
	else if (currentChar == '[') {
		tempToken.t_type = BRACKBEGIN;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// ]
	else if (currentChar == ']') {
		tempToken.t_type = BRACKEND;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// = or ==
	else if (currentChar == '=') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(tempStream);

		tempToken.lineNum = currentLineNumber;

		if (nextChar == '=') {
			tempToken.t_type = DOUBLEEQUAL;
		}
		else {
			tempToken.t_type = EQUALS;
			ungetc(nextChar, tempStream);
		}
	}

	// < or <=
	else if (currentChar == '<') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(tempStream);

		tempToken.lineNum = currentLineNumber;

		if (nextChar == '=') {
			tempToken.t_type = LESSEQ;
		}
		else {
			tempToken.t_type = LESS;
			ungetc(nextChar, tempStream);
		}
	}

	// > or >=
	else if (currentChar == '>') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(tempStream);

		tempToken.lineNum = currentLineNumber;

		if (nextChar == '=') {
			tempToken.t_type = GREATEQ;
		}
		else {
			tempToken.t_type = GREAT;
			ungetc(nextChar, tempStream);
		}
	}

	// !=
	else if (currentChar == '!') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(tempStream);

		tempToken.lineNum = currentLineNumber;

		if (nextChar == '=') {
			tempToken.t_type = NOTEQUAL;
		}
		else {
			tempToken.t_type = INVALID;
			ScannerError tempError("ERROR, INVALID USE OF '!'", tempToken.lineNum);
			ResultOfScan.push_back(tempError);
			ungetc(nextChar, tempStream);
		}
	}

	// +
	else if (currentChar == '+') {
		tempToken.t_type = ADD;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// -
	else if (currentChar == '-') {
		tempToken.t_type = SUB;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// *
	else if (currentChar == '*') {
		tempToken.t_type = MULT;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
	}

	// /, // or /*
	else if (currentChar == '/') {

		tempToken.t_string += currentChar;

		int nextChar;
		nextChar = getc(tempStream);

		tempToken.lineNum = currentLineNumber;

		if (nextChar == '/') {

			while (nextChar != '\n') {
				tempToken.t_string += nextChar;
				nextChar = getc(tempStream);
			}

			ungetc(nextChar, tempStream);

			tempToken = tokenScan();
		}

		// Found block comment start
		else if (nextChar == '*') {
			tempToken.t_string += nextChar;
			commentDepth++;
			nextChar = getc(tempStream);

			while (commentDepth != 0 && nextChar != -1) {
				while (nextChar != '*' && nextChar != '/' && nextChar != '\n' && nextChar != -1) {
					tempToken.t_string += nextChar;
					nextChar = getc(tempStream);
				}
				if (nextChar == '*') {
					tempToken.t_string += nextChar;
					nextChar = getc(tempStream);
					if (nextChar == '/') {
						commentDepth--;
						tempToken.t_string += nextChar;
					}
					else {
						nextChar = getc(tempStream);
						tempToken.t_string += nextChar;
					}
				}
				if (nextChar == '/') {
					tempToken.t_string += nextChar;
					nextChar = getc(tempStream);
					if (nextChar == '*') {
						commentDepth++;
						tempToken.t_string += nextChar;
					}
					else {
						nextChar = getc(tempStream);
						tempToken.t_string += nextChar;
					}
				}
				if (nextChar == '\n') {
					currentLineNumber++;
					nextChar = getc(tempStream);
				}
			}
			ungetc(nextChar, tempStream);
			if (commentDepth == 0) {
				tempToken = tokenScan();
			}
		}

		else {
			tempToken.t_type = DIVIDE;
			tempToken.t_char = currentChar;
			ungetc(nextChar, tempStream);
		}
	}

	// Invalid char
	else {
		tempToken.t_type = INVALID;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
		ScannerError tempError("ERROR, INVALID TOKEN, NO APPROPRIATE MATCH IN SCAN", tempToken.lineNum);
		ResultOfScan.push_back(tempError);
	}

	return tempToken; // Return the token we have scanned

}

// Scanner initialization constructor, takes a filepath, and opens the stream for that path
void Scanner::init(const char* filePath) {

	errno_t error = fopen_s(&tempStream, filePath, "r"); // Open read-only filestream with specified file path

	// Error in open
	if (error != 0) {
		// File cannot be read
	}
}

// Scanner deconstructor, closes the stream
Scanner::~Scanner() {
	fclose(tempStream);
}

// Function to scan in each token until the end of file was reached
//void Scanner::scanIn(const char* filePath) {
//
//	token firstToken = tokenScan(tempStream);
//
//	while (firstToken.t_type != FILEEND) {
//		tempTokenList.push_back(firstToken);
//		firstToken = tokenScan(tempStream);
//	}
//	tempTokenList.push_back(firstToken);
//}
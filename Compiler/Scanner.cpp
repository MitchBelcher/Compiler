/*

This cpp file contains the definitions for Scanner class functions, like initialization/construction and scanning to create tokens

*/

#include "Scanner.h"
#include "Errors.h"

#include <string>
#include <stdio.h>
#include <iostream>

int currentLineNumber = 1;
int commentDepth = 0;

// Simple function to return a list of tokens
vector<token> Scanner::getTokens() {
	return tempTokenList;
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

	else if (currentChar == -1) {
		tempToken.t_type = STREAMEND;
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

		//checkForReserves(tempToken); // Check to see if the token was a reserve word, if it wasn't no change is made

		tempToken.t_symbol = symbolTable->getSymbol(tempToken.t_string);
		if (tempToken.t_symbol == nullptr) {
			Symbol returnedSymbol;
			returnedSymbol.tempTokenType = tempToken.t_type;
			returnedSymbol.id = tempToken.t_string;
			returnedSymbol.isGlobal = false;
			//cout << "Adding symbol \t" << returnedSymbol.id << " with type\t" << returnedSymbol.tempSymbolType<< '\n';
			tempToken.t_symbol = symbolTable->addSymbol(returnedSymbol.id, returnedSymbol, returnedSymbol.isGlobal);
		}
		else
		{
			//cout << "found symbol \t" << tempToken.t_symbol->id << " with type\t" << tempToken.t_symbol->tempSymbolType << '\n';
		}
		tempToken.t_type = tempToken.t_symbol->tempTokenType;
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
			ScannerError tempError("ERROR, INVALID CHARACTER DEFINITION", tempToken.lineNum, tempToken.t_string);
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
			ScannerError tempError("ERROR, INVALID STRING DEFINITION", tempToken.lineNum, tempToken.t_string);
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
			tempToken.t_float = (float)atof(tempToken.t_string.c_str()); // Convert to float, and save in token float element
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

		if (nextChar == '=') {
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
			ScannerError tempError("ERROR, INVALID USE OF '!'", tempToken.lineNum, tempToken.t_string);
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
				tempToken.t_string += nextChar;

				if (nextChar == '*') {
					tempToken.t_string += nextChar;
					nextChar = getc(tempStream);
					if (nextChar == '/') {
						commentDepth--;
					}
				}
				else if (nextChar == '/') {
					tempToken.t_string += nextChar;
					nextChar = getc(tempStream);
					if (nextChar == '*') {
						commentDepth++;
					}
				}
				else if (nextChar == '\n') {
					currentLineNumber++;
				}
				nextChar = getc(tempStream);
			}
			ungetc(nextChar, tempStream);

			if (commentDepth > 0) {
				tempToken.t_type = INVALID;
			}

			else if (commentDepth == 0) {
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
		ScannerError tempError("ERROR, INVALID TOKEN, NO APPROPRIATE MATCH IN SCAN", tempToken.lineNum, tempToken.t_string);
		ResultOfScan.push_back(tempError);
	}

	if (tempToken.t_type == INVALID) {
		tempToken = tokenScan();
	}

	return tempToken; // Return the token we have scanner
}

// Scanner initialization constructor, takes a filepath, and opens the stream for that path
void Scanner::init(const char* filePath, SymTable& returnedSymbolTable) {

	errno_t error = fopen_s(&tempStream, filePath, "r"); // Open read-only filestream with specified file path

	// Error in open
	if (error != 0) {
		// File cannot be read
	}

	symbolTable = &returnedSymbolTable;
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
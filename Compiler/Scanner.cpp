/*

This cpp file contains the definitions for Scanner class functions, like initialization/construction and scanning to create tokens

*/

#include "Scanner.h"
#include "Errors.h"

#include <string>
#include <stdio.h>
#include <iostream>

int currentLineNumber = 1;	// Keep track of the line number we're at in the input file
int commentDepth = 0;		// Keep track of potential nested comments

// Function to systematically scan characters to create a final token
token Scanner::tokenScan() {

	token tempToken; // Create temporary token for scanning
	int currentChar; // Create a placeholder for the current character
	currentChar = getc(tempStream); // Get the current character from the stream

	// Check for whitespace
	while (isspace(currentChar)) {

		// Check for newline
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

	// Stream end
	else if (currentChar == -1) {
		tempToken.t_type = STREAMEND;
	}

	// Identifiers
	else if (isalpha(currentChar)) {

		tempToken.t_type = IDENTIFIER;				// Assume the token is a identifier
		tempToken.t_string = tolower(currentChar);	// Add the current token to the string (lowercase)
		tempToken.lineNum = currentLineNumber;		// Set line number

		int nextChar;					// Create a placeholder for the next character
		nextChar = getc(tempStream);	// Get the next character from the stream

		// If the next character is alpha, digit or _ , continue grabbing the next character and append it to the string (lowercase)
		while (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_') {
			tempToken.t_string += tolower(nextChar);	// Append lowercase to token string
			nextChar = getc(tempStream);				// Get the next character from the stream
		}
		ungetc(nextChar, tempStream);					// Put invalid character back on the tempStream

		tempToken.t_symbol = symbolTable->getSymbol(tempToken.t_string);	// Check the symbol tables for the token

		// If that symbol hasn't been added yet, set type, id, global flag, and add to the appropriate symbol table
		if (tempToken.t_symbol == nullptr) {
			Symbol returnedSymbol;
			returnedSymbol.tempTokenType = tempToken.t_type;
			returnedSymbol.id = tempToken.t_string;
			returnedSymbol.isGlobal = false;
			tempToken.t_symbol = symbolTable->addSymbol(returnedSymbol.id, returnedSymbol, returnedSymbol.isGlobal);
		}

		tempToken.t_type = tempToken.t_symbol->tempTokenType;	// Set token type to the type gotten from the symbol
	}

	// Characters
	else if (currentChar == 39) {

		int nextChar;					// Create a placeholder for the next character
		nextChar = getc(tempStream);	// Get the next character from the stream

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

		int nextChar;					// Create a placeholder for the next character
		nextChar = getc(tempStream);	// Get the next character from the stream

		// If the next character is any valid string character, continue grabbing the next character and append it to the string
		while (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_' || nextChar == ';' || nextChar == ':' || nextChar == '.' || nextChar == ',' || nextChar == 39 || nextChar == ' ') {
			tempToken.t_string += nextChar; // Append character to the token string 
			nextChar = getc(tempStream);	// Get the next character from the stream
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

		int nextChar;					// Create a placeholder for the next character
		nextChar = getc(tempStream);	// Get the next character from the stream

		// If next char is a digit or underscore, continue grabbing the next character and append it to the string
		while (isdigit(nextChar) || nextChar == '_') {

			// If the char is an underscore, absorb it
			if (nextChar != '_') {
				tempToken.t_string += nextChar;
			}

			nextChar = getc(tempStream); // Get the next character from the stream
		}

		// If the next char is a ., start processing it as a float value
		if (nextChar == '.') {

			tempToken.t_string += nextChar; // Append first decimal digit to the token string

			nextChar = getc(tempStream);	// Get the next character from the stream

			// If next char is a digit or underscore, continue grabbing the next character and append it to the string
			while (isdigit(nextChar) || nextChar == '_') {

				// If the char is an underscore, absorb it
				if (nextChar != '_') {
					tempToken.t_string += nextChar;
				}

				nextChar = getc(tempStream);		// Get the next character from the stream
			}

			ungetc(nextChar, tempStream);			// Put invalid character back on the temp stream
			tempToken.t_type = VALFLOAT;			// Set token type to float, since we found a .
			tempToken.lineNum = currentLineNumber;	// Set the line number
			tempToken.t_float = (float)atof(tempToken.t_string.c_str()); // Convert to float, and save in token float element
		}

		// Never found a ., value is an integer, and has ended
		else {
			ungetc(nextChar, tempStream);			// Put invalid character back on the temp stream
			tempToken.t_type = VALINT;				// Set token type to int, since we never found a .
			tempToken.lineNum = currentLineNumber;	// Set the line number
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

		tempToken.t_string += currentChar;	// Append first char to token string

		int nextChar;						// Create a placeholder for the next character
		nextChar = getc(tempStream);		// Get the next character from the stream

		tempToken.lineNum = currentLineNumber; // Set the line number

		// Check if the next char is '='
		if (nextChar == '=') {
			tempToken.t_type = SEMIEQUAL;	// Set new type
			tempToken.t_string += nextChar;	// Append '=' to token string
		}
		else {
			tempToken.t_type = COLON;		// Since no '=' was found, set type to colon
			ungetc(nextChar, tempStream);	// Put unneeded character back on the stream
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

		tempToken.t_string += currentChar;	//Append first char to token string

		int nextChar;						//Create a placeholder for the next character
		nextChar = getc(tempStream);		//Get the next character from the stream

		tempToken.lineNum = currentLineNumber; // Set the line number

		// Check if the next char is '='
		if (nextChar == '=') {
			tempToken.t_type = DOUBLEEQUAL; // Set new type
			tempToken.t_string += nextChar;	// Append '=' to token string
		}
		else {
			tempToken.t_type = EQUALS;		// Since no '=' was found, set type to equals
			ungetc(nextChar, tempStream);	// Put unneeded character back on the stream
		}
	}

	// < or <=
	else if (currentChar == '<') {

		tempToken.t_string += currentChar;	//Append first char to token string

		int nextChar;						//Create a placeholder for the next character
		nextChar = getc(tempStream);		//Get the next character from the stream

		tempToken.lineNum = currentLineNumber; // Set the line number

		// Check if the next char is '='
		if (nextChar == '=') {
			tempToken.t_type = LESSEQ;		// Set new type
			tempToken.t_string += nextChar;	// Append '=' to token string
		}									
		else {								
			tempToken.t_type = LESS;		// Since no '=' was found, set type to less than
			ungetc(nextChar, tempStream);	// Put unneeded character back on the stream
		}									
	}										

	// > or >=
	else if (currentChar == '>') {

		tempToken.t_string += currentChar;	//Append first char to token string

		int nextChar;						//Create a placeholder for the next character
		nextChar = getc(tempStream);		//Get the next character from the stream

		tempToken.lineNum = currentLineNumber; // Set the line number

		// Check if the next char is '='
		if (nextChar == '=') {
			tempToken.t_type = GREATEQ;		// Set new type
			tempToken.t_string += nextChar;	// Append '=' to token string
		}										
		else {									
			tempToken.t_type = GREAT;		// Since no '=' was found, set type to greater than
			ungetc(nextChar, tempStream);	// Put unneeded character back on the stream
		}										
	}											

	// !=
	else if (currentChar == '!') {

		tempToken.t_string += currentChar;	// Append first char to token string

		int nextChar;						// Create a placeholder for the next character
		nextChar = getc(tempStream);		// Get the next character from the stream

		tempToken.lineNum = currentLineNumber;  // Set the line number

		// Check if the next char if '='
		if (nextChar == '=') {
			tempToken.t_type = NOTEQUAL;	// Set new type
			tempToken.t_string += nextChar;	// Append '=' to token string
		}

		// Since no '=' was found, set the type to invalid, as '!' is not a valid token or character, put the unneeded character back on the stream
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

		// Found "//", meaning begin of inline comment
		if (nextChar == '/') {

			// Keep absorbing the text until we reach the end of the line
			while (nextChar != '\n') {
				tempToken.t_string += nextChar;
				nextChar = getc(tempStream);
			}

			ungetc(nextChar, tempStream);	// We reached the end of the line, put the unneeded character back on the stream

			tempToken = tokenScan();		// Get the next character
		}

		// Found "/*", meaning begin of block comment
		else if (nextChar == '*') {
			tempToken.t_string += nextChar;
			commentDepth++;					// We have begun a block comment, keep track of current depth to allow for nested block comments
			nextChar = getc(tempStream);

			// Keep absorbing the text until the comment depth has reached 0, or we reach the end of the stream
			while (commentDepth != 0 && nextChar != -1) {
				tempToken.t_string += nextChar;

				if (nextChar == '*') {
					tempToken.t_string += nextChar;
					nextChar = getc(tempStream);
					if (nextChar == '/') {
						commentDepth--;		// "*/" found, meaning end of block comment segment, decrement depth variable
					}
				}
				else if (nextChar == '/') {
					tempToken.t_string += nextChar;
					nextChar = getc(tempStream);
					if (nextChar == '*') {
						commentDepth++;		// "/*" found, meaning beginning of another block comment segment, increment depth variable
					}
				}
				else if (nextChar == '\n') {
					currentLineNumber++;	// Found end of line, but comment depth must still be != 0, increment line number and continue
				}
				nextChar = getc(tempStream);
			}
			ungetc(nextChar, tempStream); // Put the unneeded character back on the stream

			// If we have somehow finished the while loop, but comment depth has some value, the token is invalid
			if (commentDepth > 0) {
				tempToken.t_type = INVALID;
			}

			// If we finish the loop, and comment depth is 0, we can move on
			else if (commentDepth == 0) {
				tempToken = tokenScan();
			}
		}

		// Only found "/", meaning division symbol
		else {
			tempToken.t_type = DIVIDE;
			tempToken.t_char = currentChar;
			ungetc(nextChar, tempStream);
		}
	}

	// Invalid character, print error
	else {
		tempToken.t_type = INVALID;
		tempToken.t_char = currentChar;
		tempToken.lineNum = currentLineNumber;
		ScannerError tempError("ERROR, INVALID TOKEN, NO APPROPRIATE MATCH IN SCAN", tempToken.lineNum, tempToken.t_string);
		ResultOfScan.push_back(tempError);
	}

	// If we found an invalid token, get the next token in the hopes it isn't also invalid
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
		ScannerError tempError("ERROR, INPUT FILE CANNOT BE READ", -1, "");
	}

	symbolTable = &returnedSymbolTable;
}

// Scanner deconstructor, closes the stream
Scanner::~Scanner() {
	fclose(tempStream);
}
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const char* filePath = "C://Users//Guita//Documents//Compiler//testPgms//correct//test_program_minimal.src"; // filepath to read
bool isReserve = false;

// Instruction types, etc...
enum TYPES {
	PROGRAM,
	BEGIN, END,
	GLOBAL,
	PROCEDURE,
	PARENBEGIN, PARENEND,
	IS,
	SEMICOLON, COMMA, COLON,
	IN, OUT, INOUT,
	BRACKBEGIN, BRACKEND,
	AND, OR, NOT,
	IF, THEN, ELSE, FOR,
	SEMIEQUAL,
	ADD, SUB, MULT, DIVIDE,
	GREAT, LESS, GREATEQ, LESSEQ,
	EQUALS, NOTEQUAL, DOUBLEEQUAL,
	TRUE, FALSE,
	RETURN,
	IDENTIFIER,
	STRING, VALSTRING,
	BOOL, VALBOOL,
	CHAR, VALCHAR,
	FLOAT, VALFLOAT,
	INTEGER, VALINT,
	INVALID,
	FILEEND
};

// Structure to determine types in a token
struct token {

	TYPES t_type;

	bool t_bool;
	int t_int;
	string t_string;
	char t_char;
	float t_float;
};

vector<token> tokenList; // Vector of tokens

TYPES checkForReserve(string input) {
	int i = 0;
	char c;

	while (input[i])
	{
		c = input[i];
		input[i] = (tolower(c));
		i++;
	}

	if (input == "program") {
		isReserve = true;
		return TYPES::PROGRAM;
	}

	if (input == "begin") {
		isReserve = true;
		return TYPES::BEGIN;
	}

	if (input == "end") {
		isReserve = true;
		return TYPES::END;
	}

	if (input == "global") {
		isReserve = true;
		return TYPES::GLOBAL;
	}

	if (input == "procedure") {
		isReserve = true;
		return TYPES::PROCEDURE;
	}

	if (input == "in") {
		isReserve = true;
		return TYPES::IN;
	}

	if (input == "out") {
		isReserve = true;
		return TYPES::OUT;
	}

	if (input == "inout") {
		isReserve = true;
		return TYPES::INOUT;
	}

	if (input == "integer") {
		isReserve = true;
		return TYPES::INTEGER;
	}

	if (input == "float") {
		isReserve = true;
		return TYPES::FLOAT;
	}

	if (input == "bool") {
		isReserve = true;
		return TYPES::BOOL;
	}

	if (input == "char") {
		isReserve = true;
		return TYPES::CHAR;
	}

	if (input == "if") {
		isReserve = true;
		return TYPES::IF;
	}

	if (input == "then") {
		isReserve = true;
		return TYPES::THEN;
	}

	if (input == "else") {
		isReserve = true;
		return TYPES::ELSE;
	}

	if (input == "for") {
		isReserve = true;
		return TYPES::FOR;
	}

	if (input == "return") {
		isReserve = true;
		return TYPES::RETURN;
	}

	if (input == "not") {
		isReserve = true;
		return TYPES::NOT;
	}

	if (input == "is") {
		isReserve = true;
		return TYPES::IS;
	}

	if (input == "true") {
		isReserve = true;
		return TYPES::TRUE;
	}

	if (input == "false") {
		isReserve = true;
		return TYPES::FALSE;
	}

	if (input == "char") {
		isReserve = true;
		return TYPES::CHAR;
	}

	if (input == "bool") {
		isReserve = true;
		return TYPES::BOOL;
	}

	if (input == "string") {
		isReserve = true;
		return TYPES::STRING;
	}

	if (input == "float") {
		isReserve = true;
		return TYPES::FLOAT;
	}

	if (input == "integer") {
		isReserve = true;
		return TYPES::INTEGER;
	}

	if (input == "float") {
		isReserve = true;
		return TYPES::FLOAT;
	}
}

bool checkTrueFalse(string input) {
	if (input == "true") {
		return true;
	}

	if (input == "false") {
		return false;
	}
}


// Scanner function
token Scanner (FILE* stream) {

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
	}

	// Parentheses end
	else if (currentChar == ')') {
		tempToken.t_type = PARENEND;
	}

	// File end
	else if (currentChar == '.') {
		tempToken.t_type = FILEEND;
	}

	// Identifiers, reserve words or true/false
	else if (isalpha(currentChar)) {

		isReserve = false;

		tempToken.t_string += currentChar; // set up token string

		int nextChar; // create a variable for next character

		nextChar = getc(stream); // Get the next character

		// If the next character is alpha, digit or _, continue grabbing the next character and append it to the string
		while (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_') {
			tempToken.t_string += nextChar;
			nextChar = getc(stream);
		}
		ungetc(nextChar, stream); // Put invalid character back on the stream
		tempToken.t_type = checkForReserve(tempToken.t_string); // Check if word is reserve word

		// If word was not a reserve word, it was an identifier
		if (!isReserve) {
			tempToken.t_type = IDENTIFIER;
		}

		// if word was true/false, set token bool
		if (tempToken.t_type == TRUE || tempToken.t_type == FALSE) {
			tempToken.t_bool = checkTrueFalse(tempToken.t_string);
		}
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

		if (nextChar == '_') {
			nextChar = getc(stream);
		}



	}

	// And
	else if (currentChar == '&') {
		tempToken.t_type = AND;
	}

	// Or
	else if (currentChar == '|') {
		tempToken.t_type = OR;
	}

	// ;
	else if (currentChar == ';') {
		tempToken.t_type = SEMICOLON;
	}

	// ,
	else if (currentChar == ',') {
		tempToken.t_type = COMMA;
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
		}
	}

	// [
	else if (currentChar == '[') {
		tempToken.t_type = BRACKBEGIN;
	}

	// ]
	else if (currentChar == ']') {
		tempToken.t_type = BRACKEND;
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
		}
	}

	// +
	else if (currentChar == '+') {
		tempToken.t_type = ADD;
	}

	// -
	else if (currentChar == '-') {
		tempToken.t_type = SUB;
	}

	// *
	else if (currentChar == '*') {
		tempToken.t_type = MULT;
	}

	// /
	else if (currentChar == '/') {
		tempToken.t_type = DIVIDE;
	}

	// Invalid
	else {
		tempToken.t_type = INVALID;
	}

	return tempToken;
}

// Main
int main(int argc, char *argv[]) {

	FILE* stream; // Open filestream
	errno_t error = fopen_s(&stream, filePath, "r"); // Open read-only filestream with specified file path

	// Error in open
	if (error != 0) {
		// File cannot be read
	}

	// Cycle the file for token extraction
	token firstToken = Scanner(stream);
	while (firstToken.t_type != FILEEND) {
		tokenList.push_back(firstToken);
		firstToken = Scanner(stream);
	}

}
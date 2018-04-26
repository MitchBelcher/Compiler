/*

This header file contains the enumerators to classify all reserve words, types, and other important identification tags for a given token
As well as the base token structure that contains the elements that make up a token

*/

#pragma once

#include <string>
#include <vector>
#include <utility>

using namespace std;


// ENUM of all possible token types, including reserve words, built-in functions, etc...
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
	COMMENT,
	FILEEND, STREAMEND,
	GETBOOL, GETINTEGER, GETFLOAT, GETSTRING, GETCHAR,
	PUTBOOL, PUTINTEGER, PUTFLOAT, PUTSTRING, PUTCHAR
};


// ENUM for all possible symbol types
enum SYMBOL_TYPES {
	UNASSIGNED,
	SYMINTEGER,
	SYMFLOAT,
	SYMBOOL,
	SYMSTRING,
	SYMCHAR,
	PROC,
	ARRAY
};


// Structure for each symbol, including it's token type (always identifier), it's symbol type, array information, id, global flag, and potential parameters if it's a procedure
struct Symbol {
	TYPES tempTokenType = IDENTIFIER;
	SYMBOL_TYPES tempSymbolType = UNASSIGNED;
	bool isArray = false;
	int arrayUpper;
	int arrayLower;
	string id;
	bool isGlobal;
	vector<tuple<Symbol*, TYPES>> procedureParameters;
};

// Structure for each token, including a token TYPE enum, a Symbol structure, and int/float/string/char/bool values as well as a line number for the token
struct token {

	TYPES t_type;
	Symbol* t_symbol;

	bool t_bool;
	int t_int;
	string t_string;
	char t_char;
	float t_float;
	int lineNum;
};
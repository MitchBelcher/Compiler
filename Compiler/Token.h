/*

This header file contains the enumerators to classify all reserve words, types, and other important identification tags for a given token
As well as the base token structure that contains the elements that make up a token

*/

#pragma once

#include <string>

using namespace std;

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
	COMMENT,
	FILEEND
};

enum SYMBOL_TYPE
{
	UNASSIGNED = 1 << 0,
	/*INTEGER = 1 << 1,
	FLOAT = 1 << 2,
	BOOL = 1 << 3,
	STRING = 1 << 4,
	CHAR = 1 << 5,
	PROC = 1 << 6,
	ARRAY = 1 << 7*/
};

struct Symbol
{
	TYPES tempTokenType;
	int tempSymType = SYMBOL_TYPE::UNASSIGNED;
	string id;
	bool global;
};

// Structure to determine types in a token
struct token {

	TYPES t_type;

	bool t_bool;
	int t_int;
	string t_string;
	char t_char;
	float t_float;
	int lineNum;
};
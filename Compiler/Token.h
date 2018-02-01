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
	COMMENT, STBLKCOMMENT, EDBLKCOMMENT,
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
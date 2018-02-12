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

		if (tempToken.t_type != FILEEND) {
			// ERROR, MISSING FILE END
			ParsingError tempError("PARSE ERROR, MISSING '.' FOR END OF FILE", tempToken.lineNum);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		// ERROR FIRST TOKEN WAS NOT PROGRAM RESERVE WORD, VIOLATION OF LANGUAGE
		ParsingError tempError("PARSE ERROR, MISSING 'PROGRAM' AT FILE BEGIN", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
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
			// ERROR, NO IS IN PROGRAM HEADER
			ParsingError tempError("PARSE ERROR, MISSING 'IS' AT FILE BEGIN", tempToken.lineNum);
			ResultOfParse.push_back(tempError);
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
			// ERROR, NO SEMICOLON AFTER DECLARATION
			ParsingError tempError("PARSE ERROR, MISSING ';' IN DECLARATION", tempToken.lineNum);
			ResultOfParse.push_back(tempError);
		}
	}

	// CHECK FOR BEGIN
	// Check that the token is the begin reserve word
	if (tempToken.t_type == BEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED BEGIN TO TREE
	}
	else {
		// ERROR, BEGIN RESERVE WORD NOT FOUND, VIOLATION OF LANGUAGE
		ParsingError tempError("PARSE ERROR, MISSING BEGIN IN DECLARATION", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
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
			// ERROR, NO SEMICOLON, VIOLATION OF DECLARATION
			ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum);
			ResultOfParse.push_back(tempError);
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
		Assign(); // Run assign procedure
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
		ParsingError tempError("PARSE ERROR, NO VALID STATEMENT PRESENT", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
	}
}

// Procedure Declaration
void Parser::ProcDeclare() {
	ProcHead(); // Run procedure head procedure
	ProcBody(); // Run procedure body procedure
}

// Variable Declaration
void Parser::VarDeclare() {
	TypeMark(); // Run type mark procedure
	Ident(); // Run identifier procedure

	// Check for left bracket
	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT BRACKET TO TREE
		Number(); // Run number procedure

		// Check for colon
		if (tempToken.t_type == COLON) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED COLON TO TREE
			Number(); // Run number procedure

			// Check for right bracket
			if (tempToken.t_type == BRACKEND) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT BRACKET TO TREE
			}
			else {
				// ERROR, MISSING RIGHT BRACKET, VIOLATION OF VARIABLE DECLARATION
				ParsingError tempError("PARSE ERROR, MISSING '[' IN VARIABLE DECLARATION", tempToken.lineNum);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			// ERROR, MISSING COLON, VIOLATION OF VARIABLE DECLARATION
			ParsingError tempError("PARSE ERROR, MISSING ':' IN VARIABLE DECLARATION", tempToken.lineNum);
			ResultOfParse.push_back(tempError);
		}
	}
}

// Procedure Head
void Parser::ProcHead() {
	
	// Check for PROCEDURE
	if (tempToken.t_type == PROCEDURE) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED PROCEDURE TO TREE
		Ident(); // Run identifier procedure

		// Check for left parentheses
		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT PARENTHESES TO TREE
			ParamList();

			// Check for right parentheses
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT PARENTHESES TO TREE
			}
			else {
				// ERROR, RIGHT PARENTHESES MISSING, VIOLATION OF PROCEDURE CALL
				ParsingError tempError("PARSE ERROR, MISSING ')' IN PROCEDURE HEADER", tempToken.lineNum);
				ResultOfParse.push_back(tempError);
			}
		}
	}
	else {
		// ERROR, PROCEDURE NOT FOUND, VIOLATION OF PROCEDURE CALL
		ParsingError tempError("PARSE ERROR, MISSING PROCEDURE HEADER", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
	}
}

// Procedure Body
void Parser::ProcBody() {

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
			ParsingError tempError("PARSE ERROR, MISSING ';' IN DECLARATION", tempToken.lineNum);
			ResultOfParse.push_back(tempError);
		}
	}

	// CHECK FOR BEGIN
	// Check that the token is the begin reserve word
	if (tempToken.t_type == BEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED BEGIN TO TREE
	}
	else {
		// ERROR, BEGIN RESERVE WORD NOT FOUND, VIOLATION OF LANGUAGE
		ParsingError tempError("PARSE ERROR, MISSING BEGIN IN PROCEDURE BODY", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
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
			// ERROR, NO SEMICOLON, VIOLATION OF DECLARATION
			ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum);
			ResultOfParse.push_back(tempError);
		}
	}
}

// Parameter List
void Parser::ParamList() {
	Param(); // Run parameter procedure

	// Check for comma
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED COMMA TO TREE
		ParamList(); // Run parameter list procedure
	}
}

// Parameter
void Parser::Param() {
	VarDeclare(); // Run variable declaration procedure

	// Check for IN
	if (tempToken.t_type == IN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED IN TO TREE
	}

	// Check for OUT
	else if (tempToken.t_type == OUT) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED OUT TO TREE
	}

	// Check for INOUT
	else if (tempToken.t_type == INOUT) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED INOUT TO TREE
	}
}

// Type Mark
void Parser::TypeMark() {

	// Check for INTEGER
	if (tempToken.t_type == INTEGER) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED INTEGER TO TREE
	}

	// Check for FLOAT
	else if (tempToken.t_type == FLOAT) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED FLOAT TO TREE
	}

	// Check for BOOL
	else if (tempToken.t_type == BOOL) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED BOOL TO TREE
	}

	// Check for CHAR
	else if (tempToken.t_type == CHAR) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED CHAR TO TREE
	}
	else {
		// ERROR, NO VALID TYPE DECLARED
		ParsingError tempError("PARSE ERROR, NO VALID TYPE DECLARED", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
	}
}

// Assign
void Parser::Assign() {

	// Check for IDENTIFIER
	if (tempToken.t_type == IDENTIFIER) {
		Ident(); // Run identifier procedure

		// Check for left bracket
		if (tempToken.t_type == BRACKBEGIN) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT BRACKET TO TREE
			Expr(); // Run expression procedure

			// Check for right bracket
			if (tempToken.t_type == BRACKEND) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT BRACKET TO TREE
				AssignState(); // Run statement assignment procedure
			}
			else {
				// ERROR, VIOLATION IN IDENTIFIER ASSIGNMENT
				ParsingError tempError("PARSE ERROR, MISSING ']' IN ASSIGNMENT", tempToken.lineNum);
				ResultOfParse.push_back(tempError);
			}
		}

		// Check for left parentheses
		else if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT PARENTHESES TO TREE
			ArgumentList(); // Run argument list procedure

			// Check for right parenthese
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT PARENTHESES TO TREE
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ')' IN ASSIGNMENT", tempToken.lineNum);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			AssignState(); // Run statement assignment procedure
		}
	}
}

// Statement Assignment
void Parser::AssignState() {

	// Check for EQUALS
	if (tempToken.t_type == SEMIEQUAL) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED EQUALS TO TREE
		Expr(); // Run expression procdeure
	}
	else {
		// ERROR, NO EQUALS IN ASSIGNMENT
		ParsingError tempError("PARSE ERROR, MISSING ':=' IN ASSIGNMENT", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
	}
}

// Argument List
void Parser::ArgumentList() {
	Expr(); // Run expression procedure

	// Check for COMMA
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED COMMA TO TREE
		ArgumentList(); // Run argument list procedure
	}
}

// If
void Parser::If() {

	// Check for IF
	if (tempToken.t_type == IF) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED IF TO TREE

		// Check for left parentheses
		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT PARENTHESES TO TREE
			Expr(); // Run expression procedure

			// Check for right parentheses
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT PARENTHESES TO TREE

				// Check for THEN
				if (tempToken.t_type == THEN) {
					tempToken = inputScanner.tokenScan(); // Get next token, ADDED THEN TO TREE
					Statement(); // Run statement procedure

					// Check for SEMICOLON
					if (tempToken.t_type == SEMICOLON) {
						tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
					}
					else {
						// ERROR, NO SEMICOLON IN STATEMENT
						ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum);
						ResultOfParse.push_back(tempError);
					}

					// Check for IDENTIFIER, IF, FOR, or RETURN
					while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
						Statement(); // Run statement procedure

						// Check for SEMICOLON
						if (tempToken.t_type == SEMICOLON) {
							tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
						}
						else {
							// ERROR, NO SEMICOLON IN STATEMENT
							ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum);
							ResultOfParse.push_back(tempError);
						}
					}

					// Check for ELSE
					if (tempToken.t_type == ELSE) {
						tempToken = inputScanner.tokenScan(); // Get next token, ADDED ELSE TO TREE
						Statement(); // Run statement procedure

						// Check for SEMICOLON
						if (tempToken.t_type == SEMICOLON) {
							tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
						}
						else {
							// ERROR, NO SEMICOLON IN STATEMENT
							ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum);
							ResultOfParse.push_back(tempError);
						}

						// Check for IDENTIFIER, IF, FOR, RETURN
						while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
							Statement(); // Run statement procedure

							// Check for SEMICOLON
							if (tempToken.t_type == SEMICOLON) {
								tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
							}
							else {
								// ERROR, NO SEMICOLON IN STATEMENT
								ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum);
								ResultOfParse.push_back(tempError);
							}
						}
					}

					// Check for END
					if (tempToken.t_type == END) {
						tempToken = inputScanner.tokenScan(); // Get next token, ADDED END TO TREE

						// Check for IF
						if (tempToken.t_type == IF) {
							tempToken = inputScanner.tokenScan(); // Get next token, ADDED IF TO TREE
						}
						else {
							// ERROR, NO IF FOUND IN END IF
							ParsingError tempError("PARSE ERROR, MISSING IF IN END IF", tempToken.lineNum);
							ResultOfParse.push_back(tempError);
						}
					}
					else {
						// ERROR, NO END FOUND AFTER IF STATEMENT
						ParsingError tempError("PARSE ERROR, MISSING END IN IF STATEMENT", tempToken.lineNum);
						ResultOfParse.push_back(tempError);
					}
				}
				else {
					// ERROR, NO THEN FOUND AFTER IF STATEMENT
					ParsingError tempError("PARSE ERROR, MISSING THEN IN IF STATEMENT", tempToken.lineNum);
					ResultOfParse.push_back(tempError);
				}
			}
			else {
				// ERROR, MISSING RIGHT PARENTHESES AFTER IF STATEMENT CONDITION
				ParsingError tempError("PARSE ERROR, MISSING ')' IN IF STATEMENT", tempToken.lineNum);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			// ERROR, MISSING LEFT PARENTHESE AFTER IF STATEMENT FOR CONDITION
			ParsingError tempError("PARSE ERROR, MISSING '(' IN IF STATEMENT", tempToken.lineNum);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		// ERROR, MISSING IF
		ParsingError tempError("FATAL ERROR, LOOKING FOR IF, MISSING IF", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
	}
}

// Loop
void Parser::Loop() {

	// Check for FOR
	if (tempToken.t_type == FOR) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED FOR TO TREE

		// Check for left parentheses
		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT PARENTHESES TO TREE
			AssignState(); // Run statement assignment

			// Check for SEMICOLON
			if (tempToken.t_type == SEMICOLON) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
			}
			else {
				// ERROR, MISSING SEMICOLON IN FOR STATEMENT
				ParsingError tempError("PARSE ERROR, MISSING ';' IN LOOP STATEMENT", tempToken.lineNum);
				ResultOfParse.push_back(tempError);
			}
			Expr(); // Run expression procedure

			// Check for right parentheses
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT PARENTHESES TO TREE

				// Check for IDENTIFIER, IF, FOR or RETURN
				while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
					Statement(); // Run statement procedure

					// Check for SEMICOLON
					if (tempToken.t_type == SEMICOLON) {
						tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
					}
					else {
						ParsingError tempError("PARSE ERROR, MISSING ';' IN LOOP STATEMENT", tempToken.lineNum);
						ResultOfParse.push_back(tempError);
					}
				}

				// Check for END
				if (tempToken.t_type == END) {
					tempToken = inputScanner.tokenScan(); // Get next token, ADDED END TO TREE

					// Check for FOR
					if (tempToken.t_type == FOR) {
						tempToken = inputScanner.tokenScan(); // Get next token, ADDED FOR TO TREE
					}
					else {
						ParsingError tempError("PARSE ERROR, MISSING FOR IN END FOR STATEMENT", tempToken.lineNum);
						ResultOfParse.push_back(tempError);
					}
				}
				else {
					ParsingError tempError("PARSE ERROR, MISSING END IN END FOR STATEMENT", tempToken.lineNum);
					ResultOfParse.push_back(tempError);
				}
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ')' in FOR LOOP", tempToken.lineNum);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING '(' in FOR LOOP", tempToken.lineNum);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		ParsingError tempError("FATAL ERROR, LOOKING FOR FOR, MISSING FOR", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
	}
}

// Return
void Parser::Return() {

	// Check for RETURN
	if (tempToken.t_type == RETURN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED RETURN TO TREE
	}
	else {
		// ERROR, NO RETURN FOUND
		ParsingError tempError("FATAL ERROR, NO RETURN FOUND", tempToken.lineNum);
		ResultOfParse.push_back(tempError);
	}
}













// Identifier
void Parser::Ident() {
	tempToken = inputScanner.tokenScan(); // Get next token, ADDED IDENTIFIER TO TREE
}

// Expression
void Parser::Expr() {

	// Check for NOT
	if (tempToken.t_type == NOT) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED NOT TO TREE
	}

	Arith(); // Run arithmetic procedure
	ExprPrime(); // Run expression prime procedure
	return; // Return out of the function
}

// Expression Prime
void Parser::ExprPrime() {

	// Check for AND
	if (tempToken.t_type == AND) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED AND TO TREE

		Arith(); // Run arithmetic procedure
		ExprPrime(); // Run expression prime procedure
		return; // Return out of the function
	}

	// Check for OR
	else if (tempToken.t_type == OR) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED OR TO TREE
		Arith(); // Run arithmetic procedure
		ExprPrime(); // Run expression prime procedure
		return; // Return out of the function
	}
	else {
		return; // Return out of the function
	}
}

// Arithmetic
void Parser::Arith() {
	Relat(); // Run relation procedure
	ArithPrime(); // Run arithmetic prime procedure
	return; // Return out of the function
}

// Arithmetic Prime
void Parser::ArithPrime() {

	// Check for ADD
	if (tempToken.t_type == ADD) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED ADD TO TREE
		Relat(); // Run relation procedure
		ArithPrime(); // Run arithmetic prime procedure
		return; // Return out of the function
	}

	// Check for SUB
	else if (tempToken.t_type == SUB) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED SUB TO TREE
		Relat(); // Run relation procedure
		ArithPrime(); // Run arithmetic prime procedure
		return; // Return out of the function
	}
	else {
		return; // Return out of the function
	}
}

// Relation
void Parser::Relat() {
	Term(); // Run term procedure
	RelatPrime(); // Run relation prime procedure
	return; // Return out of the function
}

// Relation Prime
void Parser::RelatPrime() {

	// Check for LESS
	if (tempToken.t_type == LESS) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LESS TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return; // Return out of the function
	}

	// Check for GREAT
	else if (tempToken.t_type == GREAT) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED GREAT TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return; // Return out of the function
	}

	// Check for EQUALS
	else if (tempToken.t_type == EQUALS) {
		tempToken = inputScanner.tokenScan(); // Get next token, EQUALS GREAT TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return; // Return out of the function
	}

	// Check for NOT EQUAL
	else if (tempToken.t_type == NOTEQUAL) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED NOTEQUAL TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return; // Return out of the function
	}

	// Check for LESSEQ
	else if (tempToken.t_type == LESSEQ) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LESSEQ TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return; // Return out of the function
	}

	// Check for GREATEQ
	else if (tempToken.t_type == GREATEQ) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED GREATEQ TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return; // Return out of the function
	}
	else {
		return; // Return out of the function
	}
}

// Term
void Parser::Term() {
	Factor(); // Run factor procedure
	TermPrime(); // Run term prime procedure
	return; // Return out of the function
}

// Term Prime
void Parser::TermPrime() {

	// Check for MULT
	if (tempToken.t_type == MULT) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED MULT TO TREE
		Factor(); // Run factor procedure
		TermPrime(); // Run term prime procedure
		return; // Return out of the function
	}

	// Check for DIVIDE
	if (tempToken.t_type == DIVIDE) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED DIVIDE TO TREE
		Factor(); // Run factor procedure
		TermPrime(); // Run term prime procedure
		return; // Return out of the function
	}
	else {
		return; // Return out of the function
	}
}

// Factor
void Parser::Factor() {

	// Check for left parentheses
	if (tempToken.t_type == PARENBEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT PARENTHESES TO TREE
		Expr(); // Run expression procedure

		// Check for right parentheses
		if (tempToken.t_type == PARENEND) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT PARENTHESES TO TREE
		}
		else {
			// ERROR, NO RIGHT PARENTHESES IN FACTOR
		}
		return; // Return out of the function
	}

	// Check for SUB
	else if (tempToken.t_type == SUB) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED SUB TO TREE

		// Check for IDENTIFIER
		if (tempToken.t_type == IDENTIFIER) {
			Name(); // Run name procedure
			return; // Return out of the function
		}

		// Check for FLOAT OR INTEGER
		if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
			Number(); // Run number procedure
			return; // Return out of the function
		}
	}

	// Check for IDENTIFIER
	else if (tempToken.t_type == IDENTIFIER) {
		Name(); // Run name procedure
		return; // Return out of the function
	}

	// Check for FLOAT OR INTEGER
	else if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
		Number(); // Run number procedure
		return; // Return out of the function
	}

	// Check for STRING
	else if (tempToken.t_type == VALSTRING) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED STRING TO TREE
		return; // Return out of the function
	}

	// Check for CHAR
	else if (tempToken.t_type == VALCHAR) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED CHAR TO TREE
		return; // Return out of the function
	}

	// Check for TRUE
	else if (tempToken.t_type == TRUE) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED TRUE TO TREE
		return; // Return out of the function
	}

	// Check for FALSE
	else if (tempToken.t_type == FALSE) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED FALSE TO TREE
		return; // Return out of the function
	}
	else
	{
		return; // Return out of the function
	}
}

// Name
void Parser::Name() {
	Ident(); // Run identifier procedure

	// Check for left bracket
	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT BRACKET TO TREE
		Expr(); // Run expression procedure

		// Check for right bracket
		if (tempToken.t_type == BRACKEND) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT BRACKET TO TREE
		}
		else {
			// ERROR, NO RIGHT BRACKET IN NAME
		}
		return; // Return out of the function
	}
}

// Number
void Parser::Number() {
	tempToken = inputScanner.tokenScan(); // Get next token, ADDED NUMBER TO TREE
}
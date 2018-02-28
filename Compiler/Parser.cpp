/*

This cpp file contains the definition for Parser class functions, each going through the LL1 parse tree for this language

*/

#include "Parser.h"
#include "Errors.h"

// Main parser constructor, which calls the constructor for the scanner instance
Parser::Parser(const char* filePath, SymTable& newSymbolTable) {
	inputScanner.init(filePath, newSymbolTable);
	symbolTable = &newSymbolTable;
}

// Begin parsing the file by scanning the first token, and then running the program procedures
void Parser::parseFile() {
	tempToken = inputScanner.tokenScan();
	if (tempToken.t_type == STREAMEND) {
		ParsingError tempError("FATAL ERROR, MISMATCHED BLOCK COMMENTS", -1, "");
		ResultOfParse.push_back(tempError);
		return;
	}
	if (tempToken.t_type == INVALID) {
		ParsingError tempError("FATAL ERROR, INVALID TOKEN FOUND", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
		return;
	}
	Program();
}

// Program
DataStore Parser::Program() {

	DataStore programData;

	// If the first token is the program reserve word, we can proceed
	if (tempToken.t_type == PROGRAM) {
		ProgramHead(); // Run program header procedure
		ProgramBody(); // Rune program body procedure

		symbolTable->CloseScope();

		if (tempToken.t_type != FILEEND) {
			// ERROR, MISSING FILE END
			ParsingError tempError("FATAL ERROR, MISSING '.' FOR END OF FILE", -1, "");
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		// ERROR FIRST TOKEN WAS NOT PROGRAM RESERVE WORD, VIOLATION OF LANGUAGE
		ParsingError tempError("PARSE ERROR, MISSING 'PROGRAM' AT FILE BEGIN", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return programData;
}

// Program Head
DataStore Parser::ProgramHead() {

	DataStore programHeadData;

	// Check that the token is the program reserve word, ADDED PROGRAM TO TREE
	if (tempToken.t_type == PROGRAM) {
		symbolTable->OpenScope();
		tempToken = inputScanner.tokenScan(); // Get next token
		Ident(); // Run identifier procedure

		// Check that the token is the is reserve word
		if (tempToken.t_type == IS) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED IS TO TREE
		}
		else {
			// ERROR, NO IS IN PROGRAM HEADER
			ParsingError tempError("PARSE ERROR, MISSING 'IS' AT FILE BEGIN", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	return programHeadData;
}

// Program Body
DataStore Parser::ProgramBody() {

	DataStore programBodyData;

	// CHECK FOR DECLARATION
	// If the token has type of global, procedure, integer, float, bool, string or char
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare(); // Run declaration procedure

		//tempToken = inputScanner.tokenScan(); // Get next token

		// Check that the token is the semicolon type
		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
		}
		else {
			// ERROR, NO SEMICOLON AFTER DECLARATION
			ParsingError tempError("PARSE ERROR, MISSING ';' IN DECLARATION", tempToken.lineNum, tempToken.t_string);
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
		ParsingError tempError("PARSE ERROR, MISSING BEGIN IN DECLARATION", tempToken.lineNum, tempToken.t_string);
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
			ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	// Check for END
	if (tempToken.t_type == END) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED END TO TREE

		// Check for PROGRAM
		if (tempToken.t_type == PROGRAM) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED PROGRAM TO TREE
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING PROGRAM IN END PROGRAM", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING END IN END PROGRAM", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return programBodyData;
}

// Declare
DataStore Parser::Declare() {

	DataStore declarationData;
	bool isGlobal = false;


	// CHECK FOR GLOBAL
	if (tempToken.t_type == GLOBAL) {
		isGlobal = true;
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED GLOBAL TO TREE

		// CHECK FOR PROCEDURE
		if (tempToken.t_type == PROCEDURE) {
			ProcDeclare(isGlobal); // Run procedure declaration procedure
		}

		// Check if type is a variable
		else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
			VarDeclare(isGlobal); // Run variable declaration procedure
		}
	}

	// CHECK FOR PROCEDURE
	else if (tempToken.t_type == PROCEDURE) {
		ProcDeclare(isGlobal); // Run procedure declaration procedure
	}

	// Check if type is a variable
	else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
		VarDeclare(isGlobal); // Run variable declaration procedure
	}
	return declarationData;
}

// Statement
DataStore Parser::Statement() {

	DataStore statementData;

	// Assign statement/Procedure 
	if (tempToken.t_type == IDENTIFIER) {
		Assign(false); // Run assign procedure
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
		ParsingError tempError("PARSE ERROR, NO VALID STATEMENT PRESENT", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return statementData;
}

// Procedure Declaration
DataStore Parser::ProcDeclare(bool isGlobal) {
	DataStore procedureDeclarationData;
	ProcHead(isGlobal); // Run procedure head procedure
	ProcBody(); // Run procedure body procedure
	symbolTable->CloseScope();
	return procedureDeclarationData;
}

// Variable Declaration
DataStore Parser::VarDeclare(bool isGlobal) {
	DataStore variableDeclarationData;
	bool negBound = false;

	Symbol tempSymbol;
	tempSymbol.isGlobal = isGlobal;
	DataStore dataToHandle = TypeMark();
	tempSymbol.tempSymbolType = dataToHandle.tempType;

	Symbol* newSymbol = nullptr;
	dataToHandle = Ident();
	variableDeclarationData.tempToken = dataToHandle.tempToken;
	tempSymbol.id = variableDeclarationData.tempToken.t_string;
	newSymbol = variableDeclarationData.tempToken.t_symbol;

	// Check for left bracket
	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT BRACKET TO TREE
		if (tempToken.t_type == SUB) {
			tempToken = inputScanner.tokenScan();
			negBound = true;
		}
		dataToHandle = Number(); // Run number procedure

		if (dataToHandle.tempToken.t_type != VALINT) {
			ParsingError tempError("PARSE ERROR, LOWER ARRAY BOUND MUST BE INT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
		else {
			variableDeclarationData.tempToken = dataToHandle.tempToken;
			if (negBound) {
				variableDeclarationData.tempToken.t_int = dataToHandle.tempToken.t_int * -1;
			}
			tempSymbol.arrayLower = dataToHandle.tempToken.t_int;
		}

		// Check for colon
		if (tempToken.t_type == COLON) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED COLON TO TREE
			if (tempToken.t_type == SUB) {
				tempToken = inputScanner.tokenScan();
				negBound = true;
			}
			dataToHandle = Number(); // Run number procedure
			if (dataToHandle.tempToken.t_int != VALINT) {
				ParsingError tempError("PARSE ERROR, UPPER ARRAY BOUND MUST BE INT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
			else {
				if (!(tempSymbol.arrayLower < dataToHandle.tempToken.t_int)) {
					ParsingError tempError("PARSE ERROR, UPPER ARRAY BOUND MUST BE GREATER THAN LOWER", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
				else {
					if (negBound) {
						variableDeclarationData.tempToken.t_int = dataToHandle.tempToken.t_int * -1;
					}
					tempSymbol.arrayUpper = dataToHandle.tempToken.t_int;
				}
			}

			// Check for right bracket
			if (tempToken.t_type == BRACKEND) {
				tempSymbol.isArray = true;
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT BRACKET TO TREE
			}
			else {
				// ERROR, MISSING RIGHT BRACKET, VIOLATION OF VARIABLE DECLARATION
				ParsingError tempError("PARSE ERROR, MISSING '[' IN VARIABLE DECLARATION", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			// ERROR, MISSING COLON, VIOLATION OF VARIABLE DECLARATION
			ParsingError tempError("PARSE ERROR, MISSING ':' IN VARIABLE DECLARATION", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	if (tempSymbol.isGlobal == newSymbol->isGlobal) {
		if (newSymbol->tempSymbolType == UNASSIGNED) {
			newSymbol->tempSymbolType = tempSymbol.tempSymbolType;
			newSymbol->isArray = tempSymbol.isArray;
			newSymbol->arrayLower = tempSymbol.arrayLower;
			newSymbol->arrayUpper = tempSymbol.arrayUpper;
		}
		else {
			ParsingError tempError("PARSE ERROR, REDECLARING VARIABLE", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		if (tempSymbol.isGlobal == false) {
			variableDeclarationData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, tempSymbol.isGlobal);
		}
		else {
			Symbol* checkGlobal = symbolTable->getSymbol(tempSymbol.id, true);
			if (checkGlobal == nullptr) {
				variableDeclarationData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, tempSymbol.isGlobal);
			}
			else {
				ParsingError tempError("PARSE ERROR, REDECLARING VARIABLE IN GLOBAL SCOPE", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
	}
	return variableDeclarationData;
}

// Procedure Head
DataStore Parser::ProcHead(bool isGlobal) {
	
	DataStore procedureHeaderData;

	// Check for PROCEDURE
	if (tempToken.t_type == PROCEDURE) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED PROCEDURE TO TREE

		Symbol tempSymbol;
		tempSymbol.isGlobal = isGlobal;
		tempSymbol.tempSymbolType = PROC;

		Symbol* newSymbol = nullptr;
		DataStore dataToHandle = Ident();
		procedureHeaderData.tempToken = dataToHandle.tempToken;
		tempSymbol.id = procedureHeaderData.tempToken.t_string;
		newSymbol = procedureHeaderData.tempToken.t_symbol;

		symbolTable->OpenScope();

		// Check for left parentheses
		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT PARENTHESES TO TREE

			// Check for right parentheses
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT PARENTHESES TO TREE
			}
			else {
				DataStore paramListData = ParamList();
				tempSymbol.procedureParameters = paramListData.procedureParameters;

				if (tempToken.t_type == PARENEND) {
					tempToken = inputScanner.tokenScan();
				}
				else {
					ParsingError tempError("PARSE ERROR, MISSING ')' IN PROCEDURE HEADER", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
			}

			if (tempSymbol.isGlobal == newSymbol->isGlobal) {
				if (newSymbol->tempSymbolType == UNASSIGNED) {
					newSymbol->tempSymbolType = tempSymbol.tempSymbolType;
					newSymbol->procedureParameters = tempSymbol.procedureParameters;
				}
				else {
					ParsingError tempError("PARSE ERROR, REDECLARING VARIABLE", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
			}
			else {
				if (tempSymbol.isGlobal == false) {
					procedureHeaderData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, tempSymbol.isGlobal);
				}
				else {
					Symbol* checkGlobal = symbolTable->getSymbol(tempSymbol.id, true);
					if (checkGlobal == nullptr) {
						procedureHeaderData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, tempSymbol.isGlobal);
					}
					else {
						ParsingError tempError("PARSE ERROR, REDECLARING VARIABLE IN GLOBAL SCOPE", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}
				}
			}
		}
	}
	else {
		// ERROR, PROCEDURE NOT FOUND, VIOLATION OF PROCEDURE CALL
		ParsingError tempError("PARSE ERROR, MISSING PROCEDURE HEADER", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return procedureHeaderData;
}

// Procedure Body
DataStore Parser::ProcBody() {

	DataStore procedureBodyData;

	// CHECK FOR DECLARATION
	// If the token has type of global, procedure, integer, float, bool, string or char
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare(); // Run declaration procedure

		// Check that the token is the semicolon type
		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ';' IN DECLARATION", tempToken.lineNum, tempToken.t_string);
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
		ParsingError tempError("PARSE ERROR, MISSING BEGIN IN PROCEDURE BODY", tempToken.lineNum, tempToken.t_string);
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
			ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	// Check for END
	if (tempToken.t_type == END) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED END TO TREE

		// Check for PROCEDURE
		if (tempToken.t_type == PROCEDURE) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED PROGRAM TO TREE
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING PROCEDURE IN END PROCEDURE", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING END IN END PROCEDURE", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return procedureBodyData;
}

// Parameter List
DataStore Parser::ParamList() {
	DataStore parameterListData;

	DataStore dataFromParam = Param(); // Run parameter procedure
	parameterListData.procedureParameters.push_back(dataFromParam.procedureParameters[0]);

	// Check for comma
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED COMMA TO TREE

		dataFromParam = ParamList(); // Run parameter list procedure
		parameterListData.procedureParameters.insert(parameterListData.procedureParameters.end(), dataFromParam.procedureParameters.begin(), dataFromParam.procedureParameters.end());
	}
	return parameterListData;
}

// Parameter
DataStore Parser::Param() {
	DataStore parameterData;

	Symbol* newSymbol = nullptr;
	DataStore dataFromVariableDeclaration = VarDeclare(false); // Run variable declaration procedure
	newSymbol = dataFromVariableDeclaration.tempToken.t_symbol;

	// Check for IN
	if (tempToken.t_type == IN) {
		parameterData.procedureParameters.push_back({ newSymbol, INTYPE });
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED IN TO TREE
	}

	// Check for OUT
	else if (tempToken.t_type == OUT) {
		parameterData.procedureParameters.push_back({ newSymbol, OUTTYPE });
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED OUT TO TREE
	}

	// Check for INOUT
	else if (tempToken.t_type == INOUT) {
		parameterData.procedureParameters.push_back({ newSymbol, INOUTTYPE });
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED INOUT TO TREE
	}
	return parameterData;
}

// Type Mark
DataStore Parser::TypeMark() {

	DataStore typeData;

	// Check for INTEGER
	if (tempToken.t_type == INTEGER) {
		typeData.tempType = SYMINTEGER;
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED INTEGER TO TREE
	}

	// Check for FLOAT
	else if (tempToken.t_type == FLOAT) {
		typeData.tempType = SYMFLOAT;
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED FLOAT TO TREE
	}

	// Check for BOOL
	else if (tempToken.t_type == BOOL) {
		typeData.tempType = SYMBOOL;
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED BOOL TO TREE
	}

	// Check for CHAR
	else if (tempToken.t_type == CHAR) {
		typeData.tempType = SYMCHAR;
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED CHAR TO TREE
	}

	// Check for STRING
	else if (tempToken.t_type == STRING) {
		typeData.tempType = SYMSTRING;
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED STRING TO TREE
	}
	else {
		// ERROR, NO VALID TYPE DECLARED
		ParsingError tempError("PARSE ERROR, NO VALID TYPE DECLARED", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return typeData;
}

// Assign
DataStore Parser::Assign(bool onlyAssign) {

	DataStore assignData;

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
				ParsingError tempError("PARSE ERROR, MISSING ']' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}

		// Check for left parentheses
		else if (tempToken.t_type == PARENBEGIN && onlyAssign == true) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT PARENTHESES TO TREE
			ArgumentList(); // Run argument list procedure

			// Check for right parenthese
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT PARENTHESES TO TREE
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ')' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			if (tempToken.t_type == SEMIEQUAL) {
				AssignState(); // Run statement assignment procedure
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ':=' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
	}
	return assignData;
}

// Statement Assignment
DataStore Parser::AssignState() {

	DataStore assignStateData;

	// Check for EQUALS
	if (tempToken.t_type == SEMIEQUAL) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED EQUALS TO TREE
		Expr(); // Run expression procdeure
	}
	else {
		// ERROR, NO EQUALS IN ASSIGNMENT
		ParsingError tempError("PARSE ERROR, MISSING ':=' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return assignStateData;
}

// Argument List
DataStore Parser::ArgumentList() {

	DataStore argumentListData;

	Expr(); // Run expression procedure

	// Check for COMMA
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED COMMA TO TREE
		ArgumentList(); // Run argument list procedure
	}
	return argumentListData;
}

// If
DataStore Parser::If() {

	DataStore ifData;

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
						ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
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
							ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
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
							ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
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
								ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
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
							ParsingError tempError("PARSE ERROR, MISSING IF IN END IF", tempToken.lineNum, tempToken.t_string);
							ResultOfParse.push_back(tempError);
						}
					}
					else {
						// ERROR, NO END FOUND AFTER IF STATEMENT
						ParsingError tempError("PARSE ERROR, MISSING END IN IF STATEMENT", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}
				}
				else {
					// ERROR, NO THEN FOUND AFTER IF STATEMENT
					ParsingError tempError("PARSE ERROR, MISSING THEN IN IF STATEMENT", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
			}
			else {
				// ERROR, MISSING RIGHT PARENTHESES AFTER IF STATEMENT CONDITION
				ParsingError tempError("PARSE ERROR, MISSING ')' IN IF STATEMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			// ERROR, MISSING LEFT PARENTHESE AFTER IF STATEMENT FOR CONDITION
			ParsingError tempError("PARSE ERROR, MISSING '(' IN IF STATEMENT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		// ERROR, MISSING IF
		ParsingError tempError("FATAL ERROR, LOOKING FOR IF, MISSING IF", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return ifData;
}

// Loop
DataStore Parser::Loop() {

	DataStore loopData;

	// Check for FOR
	if (tempToken.t_type == FOR) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED FOR TO TREE

		// Check for left parentheses
		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT PARENTHESES TO TREE
			Assign(true); // Run statement assignment

			// Check for SEMICOLON
			if (tempToken.t_type == SEMICOLON) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED SEMICOLON TO TREE
			}
			else {
				// ERROR, MISSING SEMICOLON IN FOR STATEMENT
				ParsingError tempError("PARSE ERROR, MISSING ';' IN LOOP STATEMENT", tempToken.lineNum, tempToken.t_string);
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
						ParsingError tempError("PARSE ERROR, MISSING ';' IN LOOP STATEMENT", tempToken.lineNum, tempToken.t_string);
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
						ParsingError tempError("PARSE ERROR, MISSING FOR IN END FOR STATEMENT", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}
				}
				else {
					ParsingError tempError("PARSE ERROR, MISSING END IN END FOR STATEMENT", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ')' in FOR LOOP", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING '(' in FOR LOOP", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		ParsingError tempError("FATAL ERROR, LOOKING FOR FOR, MISSING FOR", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return loopData;
}

// Return
DataStore Parser::Return() {

	DataStore returnData;

	// Check for RETURN
	if (tempToken.t_type == RETURN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED RETURN TO TREE
	}
	else {
		// ERROR, NO RETURN FOUND
		ParsingError tempError("FATAL ERROR, NO RETURN FOUND", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return returnData;
}













// Identifier
DataStore Parser::Ident() {
	DataStore identifierData;
	identifierData.tempToken = tempToken;
	tempToken = inputScanner.tokenScan(); // Get next token, ADDED IDENTIFIER TO TREE
	return identifierData;
}

// Expression
DataStore Parser::Expr() {

	DataStore expressionData;

	// Check for NOT
	if (tempToken.t_type == NOT) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED NOT TO TREE
	}

	Arith(); // Run arithmetic procedure
	ExprPrime(); // Run expression prime procedure
	return expressionData; // Return out of the function
}

// Expression Prime
DataStore Parser::ExprPrime() {

	DataStore expressionData;

	// Check for AND
	if (tempToken.t_type == AND) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED AND TO TREE
		Arith(); // Run arithmetic procedure
		ExprPrime(); // Run expression prime procedure
		return expressionData; // Return out of the function
	}

	// Check for OR
	else if (tempToken.t_type == OR) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED OR TO TREE
		Arith(); // Run arithmetic procedure
		ExprPrime(); // Run expression prime procedure
		return expressionData; // Return out of the function
	}
	else {
		return expressionData; // Return out of the function
	}
}

// Arithmetic
DataStore Parser::Arith() {
	DataStore arithmeticData;
	Relat(); // Run relation procedure
	ArithPrime(); // Run arithmetic prime procedure
	return arithmeticData; // Return out of the function
}

// Arithmetic Prime
DataStore Parser::ArithPrime() {

	DataStore arithmeticData;

	// Check for ADD
	if (tempToken.t_type == ADD) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED ADD TO TREE
		Relat(); // Run relation procedure
		ArithPrime(); // Run arithmetic prime procedure
		return arithmeticData; // Return out of the function
	}

	// Check for SUB
	else if (tempToken.t_type == SUB) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED SUB TO TREE
		Relat(); // Run relation procedure
		ArithPrime(); // Run arithmetic prime procedure
		return arithmeticData; // Return out of the function
	}
	else {
		return arithmeticData; // Return out of the function
	}
}

// Relation
DataStore Parser::Relat() {
	DataStore relationData;
	Term(); // Run term procedure
	RelatPrime(); // Run relation prime procedure
	return relationData; // Return out of the function
}

// Relation Prime
DataStore Parser::RelatPrime() {
	DataStore relationData;

	// Check for LESS
	if (tempToken.t_type == LESS) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LESS TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return relationData; // Return out of the function
	}

	// Check for GREAT
	else if (tempToken.t_type == GREAT) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED GREAT TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return relationData; // Return out of the function
	}

	// Check for EQUALS
	else if (tempToken.t_type == DOUBLEEQUAL) {
		tempToken = inputScanner.tokenScan(); // Get next token, DOUBLEEQUAL GREAT TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return relationData; // Return out of the function
	}

	// Check for NOT EQUAL
	else if (tempToken.t_type == NOTEQUAL) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED NOTEQUAL TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return relationData; // Return out of the function
	}

	// Check for LESSEQ
	else if (tempToken.t_type == LESSEQ) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LESSEQ TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return relationData; // Return out of the function
	}

	// Check for GREATEQ
	else if (tempToken.t_type == GREATEQ) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED GREATEQ TO TREE
		Term(); // Run term procedure
		RelatPrime(); // Run relation prime procedure
		return relationData; // Return out of the function
	}
	else {
		return relationData; // Return out of the function
	}
}

// Term
DataStore Parser::Term() {
	DataStore termData;
	Factor(); // Run factor procedure
	TermPrime(); // Run term prime procedure
	return termData; // Return out of the function
}

// Term Prime
DataStore Parser::TermPrime() {
	DataStore termData;

	// Check for MULT
	if (tempToken.t_type == MULT) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED MULT TO TREE
		Factor(); // Run factor procedure
		TermPrime(); // Run term prime procedure
		return termData; // Return out of the function
	}

	// Check for DIVIDE
	if (tempToken.t_type == DIVIDE) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED DIVIDE TO TREE
		Factor(); // Run factor procedure
		TermPrime(); // Run term prime procedure
		return termData; // Return out of the function
	}
	else {
		return termData; // Return out of the function
	}
}

// Factor
DataStore Parser::Factor() {
	DataStore factorData;

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
			ParsingError tempError("PARSE ERROR, MISSING ')' IN FACTOR", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
		return factorData; // Return out of the function
	}

	// Check for SUB
	else if (tempToken.t_type == SUB) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED SUB TO TREE

		// Check for IDENTIFIER
		if (tempToken.t_type == IDENTIFIER) {
			Name(); // Run name procedure
			return factorData; // Return out of the function
		}

		// Check for FLOAT OR INTEGER
		if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
			Number(); // Run number procedure
			return factorData; // Return out of the function
		}
	}

	// Check for IDENTIFIER
	else if (tempToken.t_type == IDENTIFIER) {
		Name(); // Run name procedure
		return factorData; // Return out of the function
	}

	// Check for FLOAT OR INTEGER
	else if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
		Number(); // Run number procedure
		return factorData; // Return out of the function
	}

	// Check for STRING
	else if (tempToken.t_type == VALSTRING) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED STRING TO TREE
		return factorData; // Return out of the function
	}

	// Check for CHAR
	else if (tempToken.t_type == VALCHAR) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED CHAR TO TREE
		return factorData; // Return out of the function
	}

	// Check for TRUE
	else if (tempToken.t_type == TRUE) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED TRUE TO TREE
		return factorData; // Return out of the function
	}

	// Check for FALSE
	else if (tempToken.t_type == FALSE) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED FALSE TO TREE
		return factorData; // Return out of the function
	}
	else
	{
		return factorData; // Return out of the function
	}
}

// Name
DataStore Parser::Name() {
	DataStore nameData;
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
			ParsingError tempError("PARSE ERROR, MISSING ']' IN NAME", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	return nameData; // Return out of the function
}

// Number
DataStore Parser::Number() {
	DataStore numberData;
	numberData.tempToken = tempToken;
	tempToken = inputScanner.tokenScan(); // Get next token, ADDED NUMBER TO TREE
	return numberData;
}
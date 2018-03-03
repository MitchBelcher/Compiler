/*

This cpp file contains the definition for Parser class functions, each going through the LL1 parse tree for this language

*/

#include "Parser.h"
#include "Errors.h"

#include <iostream>

// Main parser constructor, which calls the constructor for the scanner instance
Parser::Parser(const char* filePath, SymTable& returnedSymbolTable) {
	inputScanner.init(filePath, returnedSymbolTable);
	symbolTable = &returnedSymbolTable;
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

		//symbolTable->CloseScope();

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
	if (dataToHandle.success) {
		tempSymbol.tempSymbolType = dataToHandle.tempType;
	}

	Symbol* returnedSymbol = nullptr;
	dataToHandle = Ident();
	variableDeclarationData.tempToken = dataToHandle.tempToken;
	if (dataToHandle.success) {
		tempSymbol.id = variableDeclarationData.tempToken.t_string;
		returnedSymbol = variableDeclarationData.tempToken.t_symbol;
	}

	// Check for left bracket
	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT BRACKET TO TREE
		if (tempToken.t_type == SUB) {
			tempToken = inputScanner.tokenScan();
			negBound = true;
		}
		dataToHandle = Number(); // Run number procedure
		
		if (dataToHandle.success) {
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
		}

		// Check for colon
		if (tempToken.t_type == COLON) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED COLON TO TREE
			if (tempToken.t_type == SUB) {
				tempToken = inputScanner.tokenScan();
				negBound = true;
			}
			dataToHandle = Number(); // Run number procedure

			if (dataToHandle.success) {
				if (dataToHandle.tempToken.t_type != VALINT) {
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
			ParsingError tempError("PARSE ERROR, MISSING ':' IN ARRAY DECLARATION", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	if (tempSymbol.isGlobal == returnedSymbol->isGlobal) {
		if (returnedSymbol->tempSymbolType == UNASSIGNED) {
			returnedSymbol->tempSymbolType = tempSymbol.tempSymbolType;
			returnedSymbol->isArray = tempSymbol.isArray;
			returnedSymbol->arrayLower = tempSymbol.arrayLower;
			returnedSymbol->arrayUpper = tempSymbol.arrayUpper;
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
				if (returnedSymbol->tempSymbolType == UNASSIGNED) {
					returnedSymbol->tempSymbolType = tempSymbol.tempSymbolType;
					returnedSymbol->isArray = tempSymbol.isArray;
					returnedSymbol->arrayLower = tempSymbol.arrayLower;
					returnedSymbol->arrayUpper = tempSymbol.arrayUpper;
				}
				else {
					ParsingError tempError("PARSE ERROR, REDECLARING VARIABLE IN GLOBAL SCOPE", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
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

		Symbol* returnedSymbol = nullptr;
		DataStore dataToHandle = Ident();
		procedureHeaderData.tempToken = dataToHandle.tempToken;
		if (dataToHandle.success) {
			tempSymbol.id = procedureHeaderData.tempToken.t_string;
			returnedSymbol = procedureHeaderData.tempToken.t_symbol;
		}

		/*if (tempSymbol.isGlobal == returnedSymbol->isGlobal) {
			if (returnedSymbol->tempSymbolType == UNASSIGNED) {
				returnedSymbol->tempSymbolType = tempSymbol.tempSymbolType;
				returnedSymbol->procedureParameters = tempSymbol.procedureParameters;
			}
			else {
				ParsingError tempError("PARSE ERROR, REDECLARING PROCEDURE", tempToken.lineNum, tempToken.t_string);
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
					ParsingError tempError("PARSE ERROR, REDECLARING PROCEDURE IN GLOBAL SCOPE", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
			}
		}*/

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
				if (paramListData.success) {
					tempSymbol.procedureParameters = paramListData.procedureParameters;
				}

				if (tempToken.t_type == PARENEND) {
					tempToken = inputScanner.tokenScan();
				}
				else {
					ParsingError tempError("PARSE ERROR, MISSING ')' IN PROCEDURE HEADER", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
			}

			if (tempSymbol.isGlobal == returnedSymbol->isGlobal) {
				if (returnedSymbol->tempSymbolType == UNASSIGNED) {
					returnedSymbol->tempSymbolType = tempSymbol.tempSymbolType;
					returnedSymbol->procedureParameters = tempSymbol.procedureParameters;
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
		else {
			ParsingError tempError("PARSE ERROR, MISSING '(' IN PROCEDURE HEADER", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
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
	if (dataFromParam.success) {
		parameterListData.procedureParameters.push_back(dataFromParam.procedureParameters[0]);
	}

	// Check for comma
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED COMMA TO TREE

		dataFromParam = ParamList(); // Run parameter list procedure
		if (dataFromParam.success) {
			parameterListData.procedureParameters.insert(parameterListData.procedureParameters.end(), dataFromParam.procedureParameters.begin(), dataFromParam.procedureParameters.end());
		}
	}
	return parameterListData;
}

// Parameter
DataStore Parser::Param() {
	DataStore parameterData;

	Symbol* returnedSymbol = nullptr;
	DataStore dataFromVariableDeclaration = VarDeclare(false); // Run variable declaration procedure
	if (dataFromVariableDeclaration.success) {
		returnedSymbol = dataFromVariableDeclaration.tempToken.t_symbol;
	}

	// Check for IN
	if (tempToken.t_type == IN) {
		parameterData.procedureParameters.push_back({ returnedSymbol, INTYPE });
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED IN TO TREE
	}

	// Check for OUT
	else if (tempToken.t_type == OUT) {
		parameterData.procedureParameters.push_back({ returnedSymbol, OUTTYPE });
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED OUT TO TREE
	}

	// Check for INOUT
	else if (tempToken.t_type == INOUT) {
		parameterData.procedureParameters.push_back({ returnedSymbol, INOUTTYPE });
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
	DataStore destData;

	// Check for IDENTIFIER
	if (tempToken.t_type == IDENTIFIER) {
		DataStore dataToHandle = Ident(); // Run identifier procedure
		if (dataToHandle.success) {
			assignData.tempToken = dataToHandle.tempToken;
			assignData.tempType = dataToHandle.tempType;
			destData = dataToHandle;
		}

		// Check for left bracket
		if (tempToken.t_type == BRACKBEGIN) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT BRACKET TO TREE
			dataToHandle = Expr(); // Run expression procedure

			if (dataToHandle.tempType != SYMINTEGER) {
				ParsingError tempError("PARSE ERROR, ARRAY ACESSORS MUST BE INTEGER VALUES", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

			// Check for right bracket
			if (tempToken.t_type == BRACKEND) {
				tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT BRACKET TO TREE
				AssignState(destData.tempToken, destData.tempType); // Run statement assignment procedure
			}
			else {
				// ERROR, VIOLATION IN IDENTIFIER ASSIGNMENT
				ParsingError tempError("PARSE ERROR, MISSING ']' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}

		// Check for left parentheses
		else if (tempToken.t_type == PARENBEGIN && onlyAssign == false) {
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
				AssignState(destData.tempToken, destData.tempType); // Run statement assignment procedure
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
DataStore Parser::AssignState(token destTok, SYMBOL_TYPES destType) {

	DataStore assignStateData;
	DataStore dataToHandle;

	// Check for EQUALS
	if (tempToken.t_type == SEMIEQUAL) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED EQUALS TO TREE
		dataToHandle = Expr(); // Run expression procdeure
		if (dataToHandle.success) {
			assignStateData.tempToken = dataToHandle.tempToken;
			assignStateData.tempType = dataToHandle.tempType;
		}

		if (destType == dataToHandle.tempType) {

		}
		else if ((destType == SYMINTEGER && dataToHandle.tempType == SYMFLOAT) || (destType == SYMINTEGER && dataToHandle.tempType == SYMBOOL)) {

		}
		else if (destType == SYMFLOAT && dataToHandle.tempType == SYMINTEGER) {

		}
		else if (destType == SYMBOOL && dataToHandle.tempType == SYMINTEGER) {

		}
		else {
			ParsingError tempError("PARSE ERROR, INCOMPATIBLE TYPES IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
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
			DataStore dataToHandle = Expr(); // Run expression procedure
			if (dataToHandle.success) {
				ifData.tempToken = dataToHandle.tempToken;
				ifData.tempType = dataToHandle.tempType;
			}

			if (dataToHandle.tempType == SYMBOOL || dataToHandle.tempType == SYMINTEGER) {
				if (dataToHandle.tempType == SYMINTEGER) {

				}
			}
			else {
				ParsingError tempError("PARSE ERROR, IF CONDITION MUST RESOLVE TO A BOOLEAN", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

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
						ParsingError tempError("PARSE ERROR, MISSING ';' AFTER THEN IN STATEMENT", tempToken.lineNum, tempToken.t_string);
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
							ParsingError tempError("PARSE ERROR, MISSING ';' AFTER IDENT/IF/FOR/RETURN IN STATEMENT", tempToken.lineNum, tempToken.t_string);
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
							ParsingError tempError("PARSE ERROR, MISSING ';' AFTER ELSE IN STATEMENT", tempToken.lineNum, tempToken.t_string);
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
								ParsingError tempError("PARSE ERROR, MISSING ';' AFTER IDENT/IF/FOR/RETURN IN STATEMENT", tempToken.lineNum, tempToken.t_string);
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
			DataStore dataToHandle = Expr(); // Run expression procedure
			if (dataToHandle.success) {
				loopData.tempToken = dataToHandle.tempToken;
				loopData.tempType = dataToHandle.tempType;
			}

			if (dataToHandle.tempType == SYMBOOL || dataToHandle.tempType == SYMINTEGER) {
				if (dataToHandle.tempType == SYMINTEGER) {

				}
			}
			else {
				ParsingError tempError("PARSE ERROR, FOR LOOP STATEMENT MUST RESOLVE TO A BOOLEAN", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

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
	identifierData.tempType = tempToken.t_symbol->tempSymbolType;
	tempToken = inputScanner.tokenScan(); // Get next token, ADDED IDENTIFIER TO TREE
	return identifierData;
}

// Expression
DataStore Parser::Expr() {

	DataStore expressionData;
	DataStore dataToHandle;

	// Check for NOT
	if (tempToken.t_type == NOT) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = Arith();
		if (dataToHandle.success) {
			expressionData = dataToHandle;
		}
		if (expressionData.tempType != SYMINTEGER || expressionData.tempType != SYMBOOL) {
			ParsingError tempError("PARSE ERROR, USAGE OF 'NOT' NOT SUPPORTED WITH NON-INT/BOOL TYPES", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		dataToHandle = Arith();
		if (dataToHandle.success) {
			expressionData = dataToHandle;
		}
		dataToHandle = ExprPrime(dataToHandle.tempToken, dataToHandle.tempType);
		if (dataToHandle.success) {
			expressionData.tempToken = dataToHandle.tempToken;
			expressionData.tempType = dataToHandle.tempType;
		}
	}
	return expressionData;
}

// Expression Prime
DataStore Parser::ExprPrime(token prevFacTok, SYMBOL_TYPES prevFacType) {

	DataStore expressionData;
	DataStore dataToHandle;

	// Check for AND or OR
	if (tempToken.t_type == AND || tempToken.t_type == OR) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = Arith();
		if (dataToHandle.success) {
			expressionData = dataToHandle;
		}
		dataToHandle = ExprPrime(dataToHandle.tempToken, dataToHandle.tempType);
		if (dataToHandle.success) {
			expressionData.tempToken = dataToHandle.tempToken;
		}
	}
	else {
		expressionData.success = false;
		return expressionData;
	}

	if ((prevFacType == SYMBOOL && dataToHandle.tempType == SYMBOOL) || (prevFacType == SYMINTEGER && dataToHandle.tempType == SYMINTEGER)) {
		if (dataToHandle.tempType == SYMBOOL) {
			expressionData.tempType = SYMBOOL;
		}
		else {
			expressionData.tempType = SYMINTEGER;
		}
	}
	else {
		ParsingError tempError("PARSE ERROR, & OR | MUST BE USED WITH BOOLS OR INTEGERS", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return expressionData;
}

// Arithmetic
DataStore Parser::Arith() {
	DataStore arithmeticData;
	DataStore dataToHandle = Relat();
	if (dataToHandle.success) {
		arithmeticData = dataToHandle;
	}
	dataToHandle = ArithPrime(dataToHandle.tempToken, dataToHandle.tempType);
	if (dataToHandle.success) {
		arithmeticData.tempToken = dataToHandle.tempToken;
		arithmeticData.tempType = dataToHandle.tempType;
	}
	return arithmeticData;
}

// Arithmetic Prime
DataStore Parser::ArithPrime(token prevFacTok, SYMBOL_TYPES prevFacType) {

	DataStore arithmeticData;
	DataStore dataToHandle;
	 
	// Check for ADD or SUB
	if (tempToken.t_type == ADD || tempToken.t_type == SUB) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = Relat();
		if (dataToHandle.success) {
			arithmeticData = dataToHandle;
		}
		dataToHandle = ArithPrime(dataToHandle.tempToken, dataToHandle.tempType);
		if (dataToHandle.success) {
			arithmeticData.tempToken = dataToHandle.tempToken;
		}
		return arithmeticData;
	}
	else {
		arithmeticData.success = false;
		return arithmeticData;
	}

	if ((prevFacType == SYMINTEGER && dataToHandle.tempType == SYMINTEGER) || (prevFacType == SYMFLOAT && dataToHandle.tempType == SYMFLOAT)) {
		if (dataToHandle.tempType == SYMINTEGER) {
			arithmeticData.tempType = SYMINTEGER;
		}
		else {
			arithmeticData.tempType = SYMFLOAT;
		}
	}
	else if ((prevFacType == SYMINTEGER && dataToHandle.tempType == SYMFLOAT) || (prevFacType == SYMFLOAT && dataToHandle.tempType == SYMINTEGER)) {
		if (dataToHandle.tempType == SYMINTEGER) {
			arithmeticData.tempType = SYMFLOAT;
		}
		else {
			arithmeticData.tempType = SYMFLOAT;
		}
	}
	else {
		ParsingError tempError("PARSE ERROR, CANNOT ADD OR SUBTRACT NON-INT/FLOAT TYPES", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return arithmeticData;
}

// Relation
DataStore Parser::Relat() {
	DataStore relationData;
	DataStore dataToHandle = Term();
	if (dataToHandle.success) {
		relationData = dataToHandle;
	}
	dataToHandle = RelatPrime(dataToHandle.tempToken, dataToHandle.tempType);
	if (dataToHandle.success) {
		relationData.tempToken = dataToHandle.tempToken;
		relationData.tempType = dataToHandle.tempType;
	}
	return relationData;
}

// Relation Prime
DataStore Parser::RelatPrime(token prevFacTok, SYMBOL_TYPES prevFacType) {
	DataStore relationData;
	DataStore dataToHandle;

	// Check for relational operators
	if (tempToken.t_type == LESS || tempToken.t_type == GREAT || tempToken.t_type == DOUBLEEQUAL || tempToken.t_type == NOTEQUAL || tempToken.t_type == LESSEQ || tempToken.t_type == GREATEQ) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = Term();
		if (dataToHandle.success) {
			relationData = dataToHandle;
		}
		dataToHandle = RelatPrime(dataToHandle.tempToken, dataToHandle.tempType);
		if (dataToHandle.success) {
			relationData.tempToken = dataToHandle.tempToken;
			relationData.tempType = dataToHandle.tempType;
		}
		return relationData;
	}
	else {
		relationData.success = false;
		return relationData;
	}

	if ((prevFacType == SYMBOOL && dataToHandle.tempType == SYMBOOL) || (prevFacType == SYMINTEGER && dataToHandle.tempType == SYMINTEGER) || (prevFacType == SYMFLOAT && dataToHandle.tempType == SYMFLOAT)) {

	}
	else if ((prevFacType == SYMBOOL && dataToHandle.tempType == SYMINTEGER) || (prevFacType == SYMINTEGER && dataToHandle.tempType == SYMBOOL)) {
		if (prevFacType == SYMBOOL) {

		}
		else {

		}
	}
	else if ((prevFacType == SYMINTEGER && dataToHandle.tempType == SYMFLOAT) || (prevFacType == SYMFLOAT && dataToHandle.tempType == SYMINTEGER)) {
		if (prevFacType == SYMINTEGER) {

		}
		else {

		}
	}
	else {
		ParsingError tempError("PARSE ERROR, MUST USE INT-INT, FLOAT-FLOAT, BOOL-BOOL, INT-BOOL, OR INT-FLOAT WITH RELATIONAL OPERATOR", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return relationData;
}

// Term
DataStore Parser::Term() {
	DataStore termData;
	DataStore dataToHandle = Factor();
	if (dataToHandle.success) {
		termData = dataToHandle;
	}
	dataToHandle = TermPrime(dataToHandle.tempToken, dataToHandle.tempType);
	if (dataToHandle.success) {
		termData.tempToken = dataToHandle.tempToken;
		termData.tempType = dataToHandle.tempType;
	}
	return termData;
}

// Term Prime
DataStore Parser::TermPrime(token prevFacTok, SYMBOL_TYPES prevFacType) {
	DataStore termData;
	DataStore dataToHandle;

	// Check for MULT
	if (tempToken.t_type == MULT || tempToken.t_type == DIVIDE) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = Factor();
		if (dataToHandle.success) {
			termData = dataToHandle;
		}
		TermPrime(dataToHandle.tempToken, dataToHandle.tempType);
		if (dataToHandle.success) {
			termData.tempToken = dataToHandle.tempToken;
			termData.tempType = dataToHandle.tempType;
		}
		return termData;
	}
	else {
		termData.success = false;
		return termData;
	}

	if (prevFacType == SYMINTEGER && dataToHandle.tempType == SYMINTEGER) {
		termData.tempType = SYMINTEGER;
	}
	else if ((prevFacType == SYMFLOAT && dataToHandle.tempType == SYMFLOAT) || (prevFacType == SYMINTEGER && dataToHandle.tempType == SYMFLOAT) || (prevFacType == SYMFLOAT && dataToHandle.tempType == SYMINTEGER)) {
		termData.tempType = SYMFLOAT;
		
		if (prevFacType == SYMINTEGER) {

		}
		else if (dataToHandle.tempType == SYMINTEGER) {

		}
	}
	else {
		ParsingError tempError("PARSE ERROR, MUST USE INTEGERS AND FLOATS WITH MULTIPLY/DIVIDE", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return termData;
}

// Factor
DataStore Parser::Factor() {
	DataStore factorData;
	DataStore dataToHandle;

	// Check for left parentheses
	if (tempToken.t_type == PARENBEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT PARENTHESES TO TREE
		dataToHandle = Expr(); // Run expression procedure
		if (dataToHandle.success) {
			factorData.tempType = dataToHandle.tempType;
		}

		// Check for right parentheses
		if (tempToken.t_type != PARENEND) {
			ParsingError tempError("PARSE ERROR, MISSING ')' IN FACTOR", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
		tempToken = inputScanner.tokenScan();
		return factorData;
	}

	// Check for SUB
	else if (tempToken.t_type == SUB) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED SUB TO TREE

		// Check for IDENTIFIER
		if (tempToken.t_type == IDENTIFIER) {
			dataToHandle = Name(); // Run name procedure
			if (dataToHandle.success) {
				factorData.tempToken = dataToHandle.tempToken;
				factorData.tempType = dataToHandle.tempType;
			}
		}

		// Check for FLOAT OR INTEGER
		if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
			dataToHandle = Number(); // Run number procedure
			if (dataToHandle.success) {
				factorData.tempToken = dataToHandle.tempToken;
				if (tempToken.t_type == VALINT) {
					factorData.tempType = SYMINTEGER;
				}
				else {
					factorData.tempType = SYMFLOAT;
				}
			}
		}
		return factorData;
	}

	// Check for IDENTIFIER
	else if (tempToken.t_type == IDENTIFIER) {
		dataToHandle = Name(); // Run name procedure
		if (dataToHandle.success) {
			factorData.tempToken = dataToHandle.tempToken;
			factorData.tempType = dataToHandle.tempType;
		}
		return factorData; // Return out of the function
	}

	// Check for FLOAT OR INTEGER
	else if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
		dataToHandle = Number(); // Run number procedure
		if (dataToHandle.success) {
			factorData.tempToken = dataToHandle.tempToken;
		}
		if (dataToHandle.tempToken.t_type == VALINT) {
			factorData.tempType = SYMINTEGER;
		}
		else {
			factorData.tempType = SYMFLOAT;
		}
		return factorData;
	}

	else if (tempToken.t_type == VALSTRING || tempToken.t_type == VALCHAR || tempToken.t_type == TRUE || tempToken.t_type == FALSE) {
		if (tempToken.t_type == VALSTRING) {
			factorData.tempToken = tempToken;
			factorData.tempType = SYMSTRING;
			tempToken = inputScanner.tokenScan();
		}
		else if (tempToken.t_type == VALCHAR) {
			factorData.tempToken = tempToken;
			factorData.tempType = SYMCHAR;
			tempToken = inputScanner.tokenScan();
		}
		else if (tempToken.t_type == TRUE || tempToken.t_type == FALSE) {
			factorData.tempToken = tempToken;
			factorData.tempType = SYMBOOL;
			tempToken = inputScanner.tokenScan();
		}
		else {
		}
		return factorData; // Return out of the function
	}
	else
	{
		factorData.success = false;
		return factorData; // Return out of the function
	}
}

// Name
DataStore Parser::Name() {
	DataStore nameData;
	DataStore dataToHandle = Ident(); // Run identifier procedure
	if (dataToHandle.success) {
		nameData.tempToken = dataToHandle.tempToken;
		nameData.tempType = dataToHandle.tempType;
	}

	// Check for left bracket
	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan(); // Get next token, ADDED LEFT BRACKET TO TREE
		dataToHandle = Expr(); // Run expression procedure

		// Check for right bracket
		if (tempToken.t_type == BRACKEND) {
			tempToken = inputScanner.tokenScan(); // Get next token, ADDED RIGHT BRACKET TO TREE
		}
		if (dataToHandle.success) {
			if (dataToHandle.tempType != SYMINTEGER) {
				ParsingError tempError("PARSE ERROR, ARRAY INDICES MUST BE INTEGER VALUES", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ']' IN NAME", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
		tempToken = inputScanner.tokenScan();
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
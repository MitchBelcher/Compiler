/*

This cpp file contains the definition for Parser class functions, each going through the LL1 parse tree for this language

*/

#include "Parser.h"
#include "Errors.h"

#include <iostream>

bool invalidAssign = false;

// Main parser constructor, which calls the constructor for the scanner instance
Parser::Parser(const char* filePath, SymTable& returnedSymbolTable) {
	inputScanner.init(filePath, returnedSymbolTable);
	symbolTable = &returnedSymbolTable;
}

// Begin parsing the file by scanning the first token, and then running the program procedures
void Parser::beginParsingFile() {
	tempToken = inputScanner.tokenScan();

	// If the first thing we process is the end of the file, then there are issues with block comments.
	if (tempToken.t_type == STREAMEND) {
		ParsingError tempError("FATAL ERROR, MISMATCHED BLOCK COMMENTS", -1, "");
		ResultOfParse.push_back(tempError);
		return;
	}

	// If the first thing we process is an invalid token, there is an issue.
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

	if (tempToken.t_type == PROGRAM) {
		ProgramHead();
		ProgramBody();

		// Check for '.' for file end signature, if not there, error
		if (tempToken.t_type != FILEEND) {
			ParsingError tempError("FATAL ERROR, MISSING '.' FOR END OF FILE", -1, "");
			ResultOfParse.push_back(tempError);
		}
		else {
			codeGenerator.EndProgram();
		}
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING 'PROGRAM' AT FILE BEGIN", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return programData;
}

// Program Head
DataStore Parser::ProgramHead() {

	DataStore programHeadData;

	if (tempToken.t_type == PROGRAM) {
		tempToken = inputScanner.tokenScan();
		programHeadData = Ident();		// Get the programs identifier

		if (tempToken.t_type == IS) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING 'IS' AT FILE BEGIN", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING 'PROGRAM' AT FILE BEGIN", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return programHeadData;
}

// Program Body
DataStore Parser::ProgramBody() {

	DataStore programBodyData;
	string lastToken;

	// Declarations
	// ALL DECLARATIONS OUTSIDE OF A PROCEDURE ARE ASSUMED TO BE GLOBAL
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare(true);	// Declaring global identifier since outside of procedures

		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ';' IN DECLARATION", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	if (tempToken.t_type == BEGIN) {
		tempToken = inputScanner.tokenScan();
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING BEGIN IN PROGRAM DECLARATION", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}

	// Statements
	while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
		lastToken = tempToken.t_string;
		Statement(true);

		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum - 1, lastToken);
			ResultOfParse.push_back(tempError);
		}
	}

	if (tempToken.t_type == END) {
		tempToken = inputScanner.tokenScan();

		if (tempToken.t_type == PROGRAM) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING PROGRAM IN END PROGRAM", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING END IN END PROGRAM", -1, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return programBodyData;
}

// Declare
DataStore Parser::Declare(bool isOnlyGlobal) {
	DataStore declarationData;

	// Global declarations
	if (tempToken.t_type == GLOBAL) {
		tempToken = inputScanner.tokenScan();

		// Procedure declaration
		if (tempToken.t_type == PROCEDURE) {
			ProcDeclare(true);
		}

		// Variable declaration
		else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
			VarDeclare(true);
		}
	}

	// Local procedure declaration
	else if (tempToken.t_type == PROCEDURE) {
		ProcDeclare(isOnlyGlobal);
	}

	// Local variable declaration
	else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
		VarDeclare(isOnlyGlobal);
	}
	return declarationData;
}

// Statement
DataStore Parser::Statement(bool acceptParen) {

	DataStore statementData;

	if (tempToken.t_type == IDENTIFIER) {

		// If we aren't dealing with a situation where there could be parentheses (procs, if, loop, etc...), choose which assignment function to use
		if (!acceptParen) {
			Assign();
		}
		else {
			ParenAssign();
		}
	}

	else if (tempToken.t_type == IF) {
		If();
	}

	else if (tempToken.t_type == FOR) {
		Loop();
	}

	else if (tempToken.t_type == RETURN) {
		Return();
	}
	else {
		ParsingError tempError("PARSE ERROR, NO VALID STATEMENT PRESENT", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return statementData;
}

// Procedure Declaration
DataStore Parser::ProcDeclare(bool isGlobal) {
	DataStore procedureDeclarationData;
	ProcHead(isGlobal);
	ProcBody();
	symbolTable->CloseScope();	// Procedure definition over, close it's scope
	return procedureDeclarationData;
}

// Variable Declaration
DataStore Parser::VarDeclare(bool isGlobal) {
	DataStore variableDeclarationData;
	bool negBound = false;
	bool missingLowBound = false;
	bool missingUpBound = false;

	Symbol tempSymbol;
	tempSymbol.isGlobal = isGlobal;
	DataStore dataToHandle = TypeMark();	// Get the type of the variable
	if (dataToHandle.success) {
		tempSymbol.tempSymbolType = dataToHandle.tempType;
	}

	Symbol* returnedSymbol = nullptr;
	dataToHandle = Ident();					// Get the identifier of the variable
	variableDeclarationData.tempToken = dataToHandle.tempToken;
	if (dataToHandle.success) {
		tempSymbol.id = variableDeclarationData.tempToken.t_string;
		returnedSymbol = variableDeclarationData.tempToken.t_symbol;
	}

	// Variable declaration is an array
	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan();

		// Check for negative lower bound of the array, if found, set flag
		if (tempToken.t_type == SUB) {
			tempToken = inputScanner.tokenScan();
			negBound = true;
		}

		if (tempToken.t_type == VALINT) {
			dataToHandle = Number();	// Get the lower bound number value
		}
		else {
			missingLowBound = true;
			tempToken.t_string = tempToken.t_char;
			ParsingError tempError("PARSE ERROR, MISSING LOWER ARRAY BOUND", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
		
		if (dataToHandle.success && !missingLowBound) {

			// Check that lower bound is an integer value, if not, error
			if (dataToHandle.tempToken.t_type != VALINT) {
				ParsingError tempError("PARSE ERROR, LOWER ARRAY BOUND MUST BE INT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
			else {
				variableDeclarationData.tempToken = dataToHandle.tempToken;

				// If the lower bound is negative, set the tokens value to negative
				if (negBound) {
					variableDeclarationData.tempToken.t_int = dataToHandle.tempToken.t_int * -1;
				}
				tempSymbol.arrayLower = variableDeclarationData.tempToken.t_int;	// Set lower bound of symbol
			}
		}

		// Check that there is a colon between array bounds in declaration
		if (tempToken.t_type == COLON) {
			tempToken = inputScanner.tokenScan();

			// If the lower bound was negative, reset flag
			if (negBound) {
				negBound = false;
			}

			// Check for negative upper bound of the array, if found, set flag
			if (tempToken.t_type == SUB) {
				tempToken = inputScanner.tokenScan();
				negBound = true;
			}

			if (tempToken.t_type == VALINT) {
				dataToHandle = Number();	// Get the upper bound number value
			}
			else {
				missingUpBound = true;
				tempToken.t_string += tempToken.t_char;
				ParsingError tempError("PARSE ERROR, MISSING UPPER ARRAY BOUND", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

			if (dataToHandle.success && !missingUpBound) {

				// Check that upper bound is an integer value, if not, error
				if (dataToHandle.tempToken.t_type != VALINT) {
					ParsingError tempError("PARSE ERROR, UPPER ARRAY BOUND MUST BE INT", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
				else {

					// Check that the upper bouond is larger than the lower bound, if not, error
					if ((!(tempSymbol.arrayLower < dataToHandle.tempToken.t_int))) {
						ParsingError tempError("PARSE ERROR, UPPER ARRAY BOUND MUST BE GREATER THAN LOWER", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}
					else {

						// If the upper bound is negative, set the tokens value to negative
						if (negBound) {
							variableDeclarationData.tempToken.t_int = dataToHandle.tempToken.t_int * -1;
						}
						tempSymbol.arrayUpper = variableDeclarationData.tempToken.t_int;	// Set upper bound of symbol
					}
				}
			}

			// Check for end bracket to array declaration, set token array flag
			if (tempToken.t_type == BRACKEND) {
				tempSymbol.isArray = true;
				tempToken = inputScanner.tokenScan();
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING '[' IN VARIABLE DECLARATION", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ':' IN ARRAY DECLARATION", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	// Check symbol global flags
	if (tempSymbol.isGlobal == returnedSymbol->isGlobal) {

		// Check is symbol is new, if so, set values
		if (returnedSymbol->tempSymbolType == UNASSIGNED) {
			returnedSymbol->tempSymbolType = tempSymbol.tempSymbolType;
			returnedSymbol->isArray = tempSymbol.isArray;
			returnedSymbol->arrayLower = tempSymbol.arrayLower;
			returnedSymbol->arrayUpper = tempSymbol.arrayUpper;
		}

		// Symbol is not new, redeclaration of local variable
		else {
			ParsingError tempError("PARSE ERROR, REDECLARING LOCAL VARIABLE", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {

		// Check if symbol is not global, if not, add it to the symbol table
		if (tempSymbol.isGlobal == false) {
			variableDeclarationData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, tempSymbol.isGlobal);
		}

		// Symbol is intended to be global
		else {
			Symbol* checkGlobal = symbolTable->getSymbol(tempSymbol.id, true);	// Check to see if symbol exists in global scope

			// Symbol was not already in global scope, add it
			if (checkGlobal == nullptr) {
				variableDeclarationData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, tempSymbol.isGlobal);
			}

			// Symbol was in global scope, but new symbol is unassigned, set values
			else {
				if (returnedSymbol->tempSymbolType == UNASSIGNED) {
					returnedSymbol->tempSymbolType = tempSymbol.tempSymbolType;
					returnedSymbol->isArray = tempSymbol.isArray;
					returnedSymbol->arrayLower = tempSymbol.arrayLower;
					returnedSymbol->arrayUpper = tempSymbol.arrayUpper;
				}

				// Symbol is redeclaration of global variable
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

	if (tempToken.t_type == PROCEDURE) {
		tempToken = inputScanner.tokenScan();

		Symbol tempSymbol;
		tempSymbol.isGlobal = isGlobal;
		tempSymbol.tempSymbolType = PROC;

		Symbol* returnedSymbol = nullptr;
		DataStore dataToHandle = Ident();	// Get the procedures identifier
		procedureHeaderData.tempToken = dataToHandle.tempToken;
		if (dataToHandle.success) {
			tempSymbol.id = procedureHeaderData.tempToken.t_string;
			returnedSymbol = procedureHeaderData.tempToken.t_symbol;
		}

		// Check symbol global flags
		if (tempSymbol.isGlobal == returnedSymbol->isGlobal) {

			// Check if symbol is new, if so, set the type to procedure and set the parameters
			if (returnedSymbol->tempSymbolType == UNASSIGNED) {
				returnedSymbol->tempSymbolType = tempSymbol.tempSymbolType;
				returnedSymbol->procedureParameters = tempSymbol.procedureParameters;
			}

			// Procedure has already been declared in this scope, error
			else {
				ParsingError tempError("PARSE ERROR, REDECLARING PROCEDURE", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}

		// Check if symbol is not global, if not, add it to the symbol table
		else {
			if (tempSymbol.isGlobal == false) {
				procedureHeaderData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, tempSymbol.isGlobal);
			}

			// Symbol is intended to be global
			else {
				Symbol* checkGlobal = symbolTable->getSymbol(tempSymbol.id, true);	// Check to see if symbol for procedure exists in global scope

				// Symbol did not exist in global scope, add it
				if (checkGlobal == nullptr) {
					procedureHeaderData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, tempSymbol.isGlobal);
				}

				// Procedure symbol is redeclaration of global scope
				else {
					ParsingError tempError("PARSE ERROR, REDECLARING PROCEDURE IN GLOBAL SCOPE", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
			}
		}

		symbolTable->OpenScope();	//	Open new scope for the procedure

		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();

			// Check for ')', meaning there are no parameters for the procedure
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();
			}

			// There are parameters for this procedure
			else {
				DataStore paramListData = ParamList();	// Get the parameters for the procedure
				if (paramListData.success) {
					procedureHeaderData.tempToken.t_symbol->procedureParameters = paramListData.procedureParameters;
				}

				// Check for ')' to end procedure declaration
				if (tempToken.t_type == PARENEND) {
					tempToken = inputScanner.tokenScan();
				}
				else {
					ParsingError tempError("PARSE ERROR, MISSING ')' IN PROCEDURE HEADER", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
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

	// Declarations
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare(false);	// Begin declaration, assume token is not global

		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ';' IN DECLARATION", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	if (tempToken.t_type == BEGIN) {
		tempToken = inputScanner.tokenScan();
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING BEGIN IN PROCEDURE BODY", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}

	// Statements
	while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
		Statement(true);

		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	if (tempToken.t_type == END) {
		tempToken = inputScanner.tokenScan();

		if (tempToken.t_type == PROCEDURE) {
			tempToken = inputScanner.tokenScan();
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

	DataStore dataFromParam = Param();	// Get the parameter type
	if (dataFromParam.success) {
		parameterListData.procedureParameters.push_back(dataFromParam.procedureParameters[0]);
	}

	// Check for comma, if one is found, there are multiple parameters
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan();

		dataFromParam = ParamList();	// Recursively call this function to continue getting the parameter types
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
	DataStore dataFromVariableDeclaration = VarDeclare(false);	// Get variable declaration from inside parameter list, which cannot be global
	if (dataFromVariableDeclaration.success) {
		returnedSymbol = dataFromVariableDeclaration.tempToken.t_symbol;
	}

	// Parameter is IN type
	if (tempToken.t_type == IN) {
		parameterData.procedureParameters.push_back({ returnedSymbol, INTYPE });
		tempToken = inputScanner.tokenScan();
	}

	// Parameter is OUT type
	else if (tempToken.t_type == OUT) {
		parameterData.procedureParameters.push_back({ returnedSymbol, OUTTYPE });
		tempToken = inputScanner.tokenScan();
	}

	// Parameter is INOUT type
	else if (tempToken.t_type == INOUT) {
		parameterData.procedureParameters.push_back({ returnedSymbol, INOUTTYPE });
		tempToken = inputScanner.tokenScan();
	}

	// No valid parameter type found, error
	else {
		ParsingError tempError("PARSE ERROR, INVALID PARAMETER TYPE", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return parameterData;
}

// Type Mark
DataStore Parser::TypeMark() {

	DataStore typeData;

	// Symbol is an integer
	if (tempToken.t_type == INTEGER) {
		typeData.tempType = SYMINTEGER;
		tempToken = inputScanner.tokenScan();
	}

	// Symbol is a float
	else if (tempToken.t_type == FLOAT) {
		typeData.tempType = SYMFLOAT;
		tempToken = inputScanner.tokenScan();
	}

	// Symbol is a boolean
	else if (tempToken.t_type == BOOL) {
		typeData.tempType = SYMBOOL;
		tempToken = inputScanner.tokenScan();
	}

	// Symbol is a character
	else if (tempToken.t_type == CHAR) {
		typeData.tempType = SYMCHAR;
		tempToken = inputScanner.tokenScan();
	}

	// Symbol is a string
	else if (tempToken.t_type == STRING) {
		typeData.tempType = SYMSTRING;
		tempToken = inputScanner.tokenScan();
	}

	// No valid type found, error
	else {
		ParsingError tempError("PARSE ERROR, NO VALID TYPE DECLARED", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return typeData;
}

// Assignment that handles parentheses
DataStore Parser::ParenAssign() {

	DataStore assignData;
	DataStore destData;

	if (tempToken.t_type == IDENTIFIER) {
		DataStore dataToHandle = Ident();	//	Get the identifier
		if (dataToHandle.success) {
			assignData.tempToken = dataToHandle.tempToken;
			assignData.tempType = dataToHandle.tempType;
			destData = dataToHandle;
		}

		// Check for '[', meaning we are assigning an array value
		if (tempToken.t_type == BRACKBEGIN) {
			tempToken = inputScanner.tokenScan();
			dataToHandle = Expr();	// Get the expression from inside the brackets
			if (dataToHandle.success) {
				destData = dataToHandle;
			}

			// Check that array index is an integer, if not, error
			if (destData.tempType != SYMINTEGER) {
				ParsingError tempError("PARSE ERROR, ARRAY ACESSORS MUST BE INTEGER VALUES", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

			if (tempToken.t_type == BRACKEND) {
				tempToken = inputScanner.tokenScan();
				AssignState(destData.tempToken, destData.tempType);
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ']' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}

		// Check for a parentheses, something like a procedure, if, or loop statement may be being called
		else if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();

			destData = ArgumentList();	// Get the list of arguments for the procedure call
			if (destData.success) {
				assignData.args = destData.args;
			}

			// Check for ')', meaning the procedure call is done, if not present, error
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ')' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

			// Check that the number of arguments in the procedure call line up with the number of parameters the procedure is declared to accept
			if (assignData.tempToken.t_symbol->procedureParameters.size() == assignData.args.size()) {

				// Loop through each parameter
				for (int i = 0; i < assignData.procedureParameters.size(); i++) {

					// Check that the types of each parameter match with the corresponding argument in the procedure call
					if (assignData.tempToken.t_symbol->procedureParameters[i].first->tempSymbolType == assignData.args[i]) {

					}

					// At least one argument for procedure call does not match the parameter type in the procedure declaration, error
					else {
						ParsingError tempError("PARSE ERROR, MISMATCHED PROCEDURE PARAMETER TYPES", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}
				}
			}

			// If there are more parameters in the procedure declaration than there are arguments in the procedure call, error
			else if (assignData.tempToken.t_symbol->procedureParameters.size() > assignData.args.size()) {
				ParsingError tempError("PARSE ERROR, TOO FEW ARGUMENTS IN PROCEDURE CALL", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

			// If there are more arguments in the procedure call than there are parameters in the procedure declaration, error
			else {
				ParsingError tempError("PARSE ERROR, TOO MANY ARGUMENTS IN PROCEDURE CALL", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}

		// We are assigning a simple variable
		else {
			if (tempToken.t_type == SEMIEQUAL) {
				AssignState(destData.tempToken, destData.tempType);	// Go into variable assignment function, passing the token and it's type in
			}
			else if (!invalidAssign && (tempToken.t_type == EQUALS || tempToken.t_type == COLON)) {
				invalidAssign = true;
				tempToken.t_type = SEMIEQUAL;

				ParsingError tempError("PARSE WARNING, ':=' MUST BE USED FOR ASSIGNMENT, THIS IS ONLY FORGIVEN ONCE", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);

				AssignState(destData.tempToken, destData.tempType);	// Go into variable assignment function, passing the token and it's type in
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ':=' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
	}
	return assignData;
}

// Basic Assign (same as ParenAssign but doesn't look for parentheses)
DataStore Parser::Assign() {

	DataStore assignData;
	DataStore destData;

	if (tempToken.t_type == IDENTIFIER) {
		DataStore dataToHandle = Ident();	//	Get the identifier
		if (dataToHandle.success) {
			assignData.tempToken = dataToHandle.tempToken;
			assignData.tempType = dataToHandle.tempType;
			destData = dataToHandle;
		}

		// Check for '[', meaning we are assigning an array value
		if (tempToken.t_type == BRACKBEGIN) {
			tempToken = inputScanner.tokenScan();
			dataToHandle = Expr();	// Get the expression from inside the brackets
			if (dataToHandle.success) {
				destData = dataToHandle;
			}

			// Check that array index is an integer, if not, error
			if (destData.tempType != SYMINTEGER) {
				ParsingError tempError("PARSE ERROR, ARRAY ACESSORS MUST BE INTEGER VALUES", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

			if (tempToken.t_type == BRACKEND) {
				tempToken = inputScanner.tokenScan();
				AssignState(destData.tempToken, destData.tempType);
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ']' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}

		// We are assigning a simple variable
		else {
			if (tempToken.t_type == SEMIEQUAL) {
				AssignState(destData.tempToken, destData.tempType);	// Go into variable assignment function, passing the token and it's type in
			}
			else if (!invalidAssign && (tempToken.t_type == EQUALS || tempToken.t_type == COLON)) {
				invalidAssign = true;
				tempToken.t_type = SEMIEQUAL;

				ParsingError tempError("PARSE WARNING, ':=' MUST BE USED FOR ASSIGNMENT, THIS IS ONLY FORGIVEN ONCE", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);

				AssignState(destData.tempToken, destData.tempType);	// Go into variable assignment function, passing the token and it's type in
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
	string assignmentString;
	assignmentString += destTok.t_string + " := ";

	if (tempToken.t_type == SEMIEQUAL || (!invalidAssign && (tempToken.t_type == EQUALS || tempToken.t_type == COLON))) {

		if (tempToken.t_type != SEMIEQUAL) {
			invalidAssign = true;
			tempToken.t_type = SEMIEQUAL;

			ParsingError tempError("PARSE WARNING, ':=' MUST BE USED FOR ASSIGNMENT, THIS IS ONLY FORGIVEN ONCE", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}

		tempToken = inputScanner.tokenScan();
		dataToHandle = Expr();	// Get the expression on the right of the ":=", and save that token and it's type
		if (dataToHandle.success) {
			assignStateData.tempToken = dataToHandle.tempToken;
			assignStateData.tempType = dataToHandle.tempType;
		}

		// If the types of both assignment variables are the same, no conversion required, simple assignment
		if (destType == dataToHandle.tempType) {

		}

		// Second type is either a float or a boolean and must be converted to an integer for assignment
		else if ((destType == SYMINTEGER && dataToHandle.tempType == SYMFLOAT) || (destType == SYMINTEGER && dataToHandle.tempType == SYMBOOL)) {

		}

		// Must convert first variable to a float
		else if (destType == SYMFLOAT && dataToHandle.tempType == SYMINTEGER) {

		}

		// Must convert first variable to a boolean
		else if (destType == SYMBOOL && dataToHandle.tempType == SYMINTEGER) {

		}

		// The types on either side of the assignment are invalid to be assigned to one another per language specification
		else {
			assignmentString += dataToHandle.tempToken.t_string;
			ParsingError tempError("PARSE ERROR, INCOMPATIBLE TYPES IN ASSIGNMENT", tempToken.lineNum, assignmentString);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING ':=' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return assignStateData;
}

// Argument List
DataStore Parser::ArgumentList() {

	DataStore argumentListData;
	DataStore dataToHandle = Expr();	// Get the argument expression
	if (dataToHandle.success) {
		argumentListData.args.push_back(dataToHandle.tempType);
	}

	// Check for comma, if one is found there are multiple arguments
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = ArgumentList();	// Recursively call this function to continue getting the arguments
		if (dataToHandle.success) {
			argumentListData.args.insert(argumentListData.args.end(), dataToHandle.args.begin(), dataToHandle.args.end());
		}
	}
	return argumentListData;
}

// If
DataStore Parser::If() {

	DataStore ifData;

	if (tempToken.t_type == IF) {
		tempToken = inputScanner.tokenScan();

		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();

			DataStore
				dataToHandle = Expr();	// Get the expression within the "if" conditional
			if (dataToHandle.success) {
				ifData.tempToken = dataToHandle.tempToken;
				ifData.tempType = dataToHandle.tempType;
			}

			// Check that the expression in the "if" conditional can resolve to boolean
			if (dataToHandle.tempType == SYMBOOL || dataToHandle.tempType == SYMINTEGER) {

				// Expression is an integer, must convert to a boolean
				if (dataToHandle.tempType == SYMINTEGER) {

				}
			}

			// The "if" conditionals expression cannot resolve to a boolean value
			else {
				ParsingError tempError("PARSE ERROR, IF CONDITION MUST RESOLVE TO A BOOLEAN", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

			// Check for ')', meaning end of the conditional
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();

				if (tempToken.t_type == THEN) {
					tempToken = inputScanner.tokenScan();
					Statement(true);	// Get the first statement within the if section

					if (tempToken.t_type == SEMICOLON) {
						tempToken = inputScanner.tokenScan();
					}
					else {
						ParsingError tempError("PARSE ERROR, MISSING ';' IN IF STATEMENT", tempToken.lineNum - 1, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}

					// So long as there are more identifiers found, nested if conditions found, for loops found, or a return statement found, keep getting those statements
					while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
						Statement(true);

						if (tempToken.t_type == SEMICOLON) {
							tempToken = inputScanner.tokenScan();
						}
						else {
							ParsingError tempError("PARSE ERROR, MISSING ';' AFTER IDENTIFER/IF/FOR/RETURN IN STATEMENT", tempToken.lineNum, tempToken.t_string);
							ResultOfParse.push_back(tempError);
						}
					}

					// Check for else condition
					if (tempToken.t_type == ELSE) {
						tempToken = inputScanner.tokenScan();
						Statement(true);	// Get the first statement within the else condition

						if (tempToken.t_type == SEMICOLON) {
							tempToken = inputScanner.tokenScan();
						}
						else {
							ParsingError tempError("PARSE ERROR, MISSING ';' AFTER ELSE IN STATEMENT", tempToken.lineNum, tempToken.t_string);
							ResultOfParse.push_back(tempError);
						}

						// So long as there are more identifiers found, nested if conditions found, for loops found, or a return statement found, keep getting those statements
						while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
							Statement(false);

							if (tempToken.t_type == SEMICOLON) {
								tempToken = inputScanner.tokenScan();
							}
							else {
								ParsingError tempError("PARSE ERROR, MISSING ';' AFTER IDENT/IF/FOR/RETURN IN STATEMENT", tempToken.lineNum, tempToken.t_string);
								ResultOfParse.push_back(tempError);
							}
						}
					}

					// Check for end of if statement
					if (tempToken.t_type == END) {
						tempToken = inputScanner.tokenScan();

						if (tempToken.t_type == IF) {
							tempToken = inputScanner.tokenScan();
						}
						else {
							ParsingError tempError("PARSE ERROR, MISSING IF IN END IF", tempToken.lineNum, tempToken.t_string);
							ResultOfParse.push_back(tempError);
						}
					}
					else {
						ParsingError tempError("PARSE ERROR, MISSING END IN IF STATEMENT", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}
				}
				else {
					ParsingError tempError("PARSE ERROR, MISSING THEN IN IF STATEMENT", tempToken.lineNum - 1, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ')' IN IF STATEMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING '(' IN IF STATEMENT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {
		ParsingError tempError("FATAL ERROR, LOOKING FOR IF, MISSING IF", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return ifData;
}

// Loop
DataStore Parser::Loop() {

	DataStore loopData;

	if (tempToken.t_type == FOR) {
		tempToken = inputScanner.tokenScan();

		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();
			Assign();

			if (tempToken.t_type == SEMICOLON) {
				tempToken = inputScanner.tokenScan();
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ';' IN LOOP STATEMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
			DataStore dataToHandle = Expr();
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

			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();

				while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
					Statement(true);

					if (tempToken.t_type == SEMICOLON) {
						tempToken = inputScanner.tokenScan();
					}
					else {
						ParsingError tempError("PARSE ERROR, MISSING ';' IN LOOP STATEMENT", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}
				}

				if (tempToken.t_type == END) {
					tempToken = inputScanner.tokenScan();

					if (tempToken.t_type == FOR) {
						tempToken = inputScanner.tokenScan();
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

	if (tempToken.t_type == RETURN) {
		tempToken = inputScanner.tokenScan();
	}
	else {
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
	tempToken = inputScanner.tokenScan();
	return identifierData;
}

// Expression
DataStore Parser::Expr() {

	DataStore expressionData;
	DataStore dataToHandle;

	if (tempToken.t_type == NOT) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = Arith();
		if (dataToHandle.success) {
			expressionData = dataToHandle;
		}
		else {
			expressionData.success = false;
		}

		// Type check
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
		else {
			expressionData.success = false;
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

	// Type check
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
	else {
		arithmeticData.success = false;
		return arithmeticData;
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

	// Type check
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
	else {
		relationData.success = false;
		return relationData;
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

	if (tempToken.t_type == LESS || tempToken.t_type == GREAT || tempToken.t_type == DOUBLEEQUAL || tempToken.t_type == NOTEQUAL || tempToken.t_type == LESSEQ || tempToken.t_type == GREATEQ) {
		tempToken = inputScanner.tokenScan();
	}
	else {
		relationData.success = false;
		return relationData;
	}

	dataToHandle = Term();
	if (dataToHandle.success) {
		relationData = dataToHandle;
	}
	dataToHandle = RelatPrime(dataToHandle.tempToken, dataToHandle.tempType);
	if (dataToHandle.success) {
		relationData.tempToken = dataToHandle.tempToken;
		relationData.tempType = dataToHandle.tempType;
	}

	// Type check
	if ((prevFacType == SYMBOOL && relationData.tempType == SYMBOOL) || (prevFacType == SYMINTEGER && relationData.tempType == SYMINTEGER) || (prevFacType == SYMFLOAT && relationData.tempType == SYMFLOAT) || (prevFacType == SYMCHAR && relationData.tempType == SYMCHAR)) {

	}
	else if ((prevFacType == SYMBOOL && relationData.tempType == SYMINTEGER) || (prevFacType == SYMINTEGER && relationData.tempType == SYMBOOL)) {
		if (prevFacType == SYMBOOL) {

		}
		else {

		}
	}
	else if ((prevFacType == SYMINTEGER && relationData.tempType == SYMFLOAT) || (prevFacType == SYMFLOAT && relationData.tempType == SYMINTEGER)) {
		if (prevFacType == SYMINTEGER) {

		}
		else {

		}
	}
	else {
		ParsingError tempError("PARSE ERROR, MUST USE INT-INT, FLOAT-FLOAT, BOOL-BOOL, INT-BOOL, OR INT-FLOAT WITH RELATIONAL OPERATOR", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}

	relationData.tempType = SYMBOOL;
	return relationData;
}

// Term
DataStore Parser::Term() {
	DataStore termData;
	DataStore dataToHandle = Factor();
	if (dataToHandle.success) {
		termData = dataToHandle;
	}
	else {
		termData.success = false;
		return termData;
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

	// Type check
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

	if (tempToken.t_type == PARENBEGIN) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = Expr();
		if (dataToHandle.success) {
			factorData.tempType = dataToHandle.tempType;
		}

		if (tempToken.t_type != PARENEND) {
			ParsingError tempError("PARSE ERROR, MISSING ')' IN FACTOR", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
		tempToken = inputScanner.tokenScan();
		return factorData;
	}

	else if (tempToken.t_type == SUB) {
		tempToken = inputScanner.tokenScan();

		if (tempToken.t_type == IDENTIFIER) {
			dataToHandle = Name();
			if (dataToHandle.success) {
				factorData.tempToken = dataToHandle.tempToken;
				factorData.tempType = dataToHandle.tempType;
			}
		}

		if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
			dataToHandle = Number();
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

	else if (tempToken.t_type == IDENTIFIER) {
		dataToHandle = Name();
		if (dataToHandle.success) {
			factorData.tempToken = dataToHandle.tempToken;
			factorData.tempType = dataToHandle.tempType;
		}
		return factorData;
	}

	else if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
		dataToHandle = Number();
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
		return factorData;
	}
	else
	{
		factorData.success = false;
		return factorData;
	}
}

// Name
DataStore Parser::Name() {
	DataStore nameData;
	DataStore dataToHandle = Ident();
	if (dataToHandle.success) {
		nameData.tempToken = dataToHandle.tempToken;
		nameData.tempType = dataToHandle.tempType;
	}

	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = Expr();

		if (tempToken.t_type == BRACKEND) {
			tempToken = inputScanner.tokenScan();
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
	}  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return nameData;
}

// Number
DataStore Parser::Number() {
	DataStore numberData;
	numberData.tempToken = tempToken;
	tempToken = inputScanner.tokenScan();
	return numberData;
}
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


/*
	PROGRAM

This function handles the main entry point for the input file
It checks that the first token of the input file is "program" per the language specification
It will then either error, or will call the function for the Program Header and then Program Body
If it doesn't error, it also checks after calling the Program Body function that we didn't receive the end of the file back

*/
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


/*
	PROGRAM HEADER

This function handles the header for every input file program
It double checks that the Program function found "program", then scans the next token
The next token should be the identifier for the program itself, so we store that
Then it checks for "is" per the language specification, if it doesn't find either "is" or "program" it errors

*/
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


/*
	PROGRAM BODY

This function handles the body for every input file program
It first checks for appropriate declaration statements, like "global", "procedure", or one of the types allowed in the language
Then it calls the Declaration function to process that declaration, and upon returning, checks that the declaration ended in a ';' per the language specification
This continues until it finds no more appropriate declarations
Then it searches for the begin statement, and errors if it is not found
Next it checks for apropriate statements within the program body, like "if", "for", or an identifier, etc...
Then it will call the Statement function to process that statement, and upon returning, checks that the statement ended in a ';' per the language specification
This continues until it finds no more appropriate statements
Now it checks for "end program", per the language specification, and will error if either are not found

*/
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


/*
	DECLARATIONS

This function handles all declarations in the input file program
First it checks to see if the declaration has been listed as global.  If it has, we call the appropriate function for declaring procedures or variables, respectively
If it was not declared as global, we will call the appropriate function for declarations, but not specifying that the procedure or variable should explicitly be global
One unique part of the implementation for this compiler as a whole, is that if something is declared outside of a procedure, but inside of a program, it is automatically made global indiscriminantly

*/
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


/*
	STATEMENTS

This function handles all statements in the input file program
We take in a boolean value that tells us whether or not we are likely to have a parentheses involved in the assignment (for something like a procedure or if statement)
This is so that if we see an identifier, we can call the appropriate assignment function, one that will deal with parentheses and one that will just deal with simple assignments
We also check for all other valid statements in a program, "If", "For", etc... and call the corresponding function to deal with that
If we don't find a valid statement token, we will error

*/
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


/*
	PROCEDURE DECLARATIONS

This function handles all procedure declarations in the input file program
It takes in a boolean letting it know if the procedure is intended to be global or not
Then it calls the Procedure Header function, telling it is it's global or not
After that it calls the Procedure Body function, then closes the procedures scope

*/
DataStore Parser::ProcDeclare(bool isGlobal) {
	DataStore procedureDeclarationData;
	ProcHead(isGlobal);
	ProcBody();
	symbolTable->CloseScope();	// Procedure definition over, close it's scope
	return procedureDeclarationData;
}


/*
	VARIABLE DECLARATIONS

This function handles all variable declarations in the input file program
It takes in a boolean value that will tell us if the variable is intended to be global or not
First, it goes and gets the type of the variable and stores that, then it gets the identifier for the variable and stores that
Then we check for a '[' which will signal that the variable declaration is for an array
If the variable is an array, we check to see if either bounds are negative, and set them accordingly
If the '[' is found, but we are missing either bound, or the ':', we will error
We also do type checking on the array bounds to verify that the array bounds are in fact integer values, if not we error
If we found '[', we finish with checking for ']', and if we don't find it, we error

After dealing with the possible array handling, we start dealing with adding the variable to the symbol table
First we check to see if the the variable in in the correct scope currently
If it is, we check if it's already been assigned, and if it hasn't, we assign everything, otherwise we error
If the scope we want to put it in is different from the symbol we found from the Identifier function, we check if this variable is to be global or not
If the variable is not meant to be global, we add it to the local symbol table
If the variable is supposed to be global, first we check the global symbol table to see if it exists, and if it does, we error
If the variable is supposed to be global, and we don't find it in that table, we check to see if we have it but haven't assigned it yet
If it hasn't been assigned, we assign it
If it has been asssigned globally already, we error

*/
DataStore Parser::VarDeclare(bool isGlobal) {
	DataStore variableDeclarationData;
	bool negBound = false;
	bool missingLowBound = false;
	bool missingUpBound = false;

	Symbol tempSymbol;
	tempSymbol.isGlobal = isGlobal;
	DataStore dataToHandle = Type();	// Get the type of the variable
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
				ParsingError tempError("PARSE ERROR, MISSING ']' IN VARIABLE DECLARATION", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ':' IN ARRAY DECLARATION", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
			tempToken = inputScanner.tokenScan();
		}
	}

	// Check scope
	if (isGlobal == returnedSymbol->isGlobal) {

		// Check is symbol is new, if so, set values
		if (returnedSymbol->tempSymbolType == UNASSIGNED) {
			returnedSymbol->tempSymbolType = tempSymbol.tempSymbolType;
			returnedSymbol->isArray = tempSymbol.isArray;
			returnedSymbol->arrayLower = tempSymbol.arrayLower;
			returnedSymbol->arrayUpper = tempSymbol.arrayUpper;
		}

		// Symbol is not new, redeclaration
		else {
			ParsingError tempError("PARSE ERROR, REDECLARING VARIABLE", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	else {

		// Check if symbol is not global, if not, add it to the symbol table
		if (isGlobal == false) {
			variableDeclarationData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, tempSymbol.isGlobal);
		}

		// Symbol is intended to be global
		else {
			Symbol* checkGlobal = symbolTable->getSymbol(tempSymbol.id, true);	// Check to see if symbol exists in global scope

			// Symbol was not already in global scope, add it
			if (checkGlobal == nullptr) {
				variableDeclarationData.tempToken.t_symbol = symbolTable->addSymbol(tempSymbol.id, tempSymbol, true);
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


/*
	PROCEDURE HEADER

This function handles the header for all procedures in the input file program
This is exactly the same implementation and steps as the Program Header function, swapping "program" for procedure

*/
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


/*
	PROCEDURE BODY

This function handles the body for all procedures in the input file program
This is exactly the same implementation and steps as the Program Body function, swapping "program" for procedure

*/
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


/*
	PARAMETER LIST

This function handles getting the list of parameters for a given procedure
First it calls the Parameter function to process the parameter, and if appropriate, will add that parameter to the vector of parameters for this procedure
Then it looks for a ',', and if it finds one, we know that there are multiple parameters for this procedure
If it finds a ',', we get the parameter and recursively call this function, continously appending the new parameters to the end of the vector for the procedure

*/
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
			for (int i = 0; i < dataFromParam.procedureParameters.size(); i++) {
				parameterListData.procedureParameters.push_back(dataFromParam.procedureParameters[i]);
			}
		}
	}
	return parameterListData;
}


/*
	PARAMETER

This function handles interpreting the parameter for a procedure
First we call the Variable Declaration function to handle the parameter value itself
Next, we check the type on the parameter token against the three outlined by the language specification (In, Out and InOut)
If we don't find one of these, we error

*/
DataStore Parser::Param() {
	DataStore parameterData;

	Symbol* returnedSymbol = nullptr;
	DataStore dataFromVariableDeclaration = VarDeclare(false);	// Get variable declaration from inside parameter list, which cannot be global
	if (dataFromVariableDeclaration.success) {
		returnedSymbol = dataFromVariableDeclaration.tempToken.t_symbol;
	}

	// Parameter is IN type
	if (tempToken.t_type == IN) {
		parameterData.procedureParameters.push_back({ returnedSymbol, IN });
		tempToken = inputScanner.tokenScan();
	}

	// Parameter is OUT type
	else if (tempToken.t_type == OUT) {
		parameterData.procedureParameters.push_back({ returnedSymbol, OUT });
		tempToken = inputScanner.tokenScan();
	}

	// Parameter is INOUT type
	else if (tempToken.t_type == INOUT) {
		parameterData.procedureParameters.push_back({ returnedSymbol, INOUT });
		tempToken = inputScanner.tokenScan();
	}

	// No valid parameter type found, error
	else {
		ParsingError tempError("PARSE ERROR, INVALID PARAMETER TYPE", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return parameterData;
}


/*
	TYPE

This function handles setting the appropriate type of a symbol based on the token type and moves on to the next token and returns
If we don't find any valid type (integer, float, bool, char, or string), we error

*/
DataStore Parser::Type() {

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


/*
	ASSIGNMENT (SUPPORTING PARENTHESES)

This function handles assignment statements in the input file program
First it checks that there is an identifier, and if there is, we call the Identifier function to store that
Next we look for a '[' signalling that we are accessing an array.
If we are, we call the Expression function on the contents of the brackets, and then type check them to verify that they are integer values
If there was no '[', we check for '(', meaning that we are assigning within a loop or some sort or a procedure or the like
Then we call the Argument List function to get the arguments in the assignment and check for the final ')' and error if we don't find one
Once we've gotten the arguments, we check that the number of parameters and the number of arguments are the same, if not we error
If they are, then we check the type on each parameter to each argument
Lastly, if we didn't find '[' or '(', we are just assigning a simple variable, so we check that there is a ':=' and then call the Assign State function to handle that
If the input file is missing the ':=', we error, but proceed anyway

*/
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
				SetAssign(destData.tempToken, destData.tempType);
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
					if (get<0>(assignData.tempToken.t_symbol->procedureParameters[i])->tempSymbolType == assignData.args[i]) {

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
				SetAssign(destData.tempToken, destData.tempType);	// Go into variable assignment function, passing the token and it's type in
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ':=' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);

				SetAssign(destData.tempToken, destData.tempType);	// Go into variable assignment function, passing the token and it's type in
			}
		}
	}
	return assignData;
}


/*
	ASSIGNMENT (BASIC)

This function handles assignment statements in the input file program
This is exactly the same as the Paren Assign function, except that this one doesn't handle procedure assignments

*/
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

			// Check for ending bracket ']'
			if (tempToken.t_type == BRACKEND) {
				tempToken = inputScanner.tokenScan();
				SetAssign(destData.tempToken, destData.tempType);
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ']' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}

		// We are assigning a simple variable
		else {
			if (tempToken.t_type == SEMIEQUAL) {
				SetAssign(destData.tempToken, destData.tempType);	// Go into variable assignment function, passing the token and it's type in
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ':=' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);

				SetAssign(destData.tempToken, destData.tempType);	// Go into variable assignment function, passing the token and it's type in
			}
		}
	}
	return assignData;
}


/*
	ASSIGN STATE

This function handles assigning values back and forth, and primarily type checking assignments
First we check that the user has intended to assign the two values
Then we call the Expression function to get the assignment value on the right of the assignment operation
Then we type check the left and right types of the assignment, and if they are not compatible via the program language specification, we error

*/
DataStore Parser::SetAssign(token destTok, SYMBOL_TYPES destType) {

	DataStore assignStateData;
	DataStore dataToHandle;
	string assignmentString;
	assignmentString += destTok.t_string + " := ";

	if (tempToken.t_type == SEMIEQUAL || (tempToken.t_type == EQUALS || tempToken.t_type == COLON)) {

		if (tempToken.t_type != SEMIEQUAL) {
			ParsingError tempError("PARSE ERROR, ':=' MUST BE USED FOR ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
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

		else if ((destType == SYMINTEGER && dataToHandle.tempType == SYMCHAR) || (destType == SYMCHAR && dataToHandle.tempType == SYMINTEGER)) {

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


/*
	ARGUMENT LIST

This function handles getting the list of arguments for a given procedure call
This is exactly the same as the Parameter List function, except this one gets the argument expressions

*/
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
			for (int i = 0; i < dataToHandle.args.size(); i++) {
				argumentListData.args.push_back(dataToHandle.args[i]);
			}
		}
	}
	return argumentListData;
}


/*
	IF STATEMENT

This function handles the if statements found in the input program file
First we check for 'if', and then check for '(', erroring if we don't find either
Then we check to see if there is an expression conditional inside the if statement, if there isn't we check for the ')' and error if we don't find it
If there is an expression within the '()' of the if statement, we type check that that statement can resolve to a boolean value per the language specification
After that, we look for 'then' and error if we don't find it, per the language specification
Then we get the first statement in the conditional, checking that there is a ';' flanking it
Now we check so see if there are any more statements inside the if conditional and execute the Statement function for each one we find
Next we check for 'else', which may or may not be there
If it isn't there, we check for 'end if' per the language specification
If there is an 'else' we check for one or more statements within the 'else' just as before with the main if conditional

*/
DataStore Parser::If() {

	DataStore ifData;

	if (tempToken.t_type == IF) {
		tempToken = inputScanner.tokenScan();

		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();

			DataStore dataToHandle = Expr();	// Get the expression within the "if" conditional
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


/*
	LOOP STATEMENT

This function handles the for loops found in the input file program
First we check for 'for', and then for a '(', after which we call the Assign function on the iterator of the loop
Next we type check that the loops iterator resolves to a boolean or a integer per the language specification
After the ')', we check for any statements that are made in the loop, calling the Statement function for each
Finally, we look for 'end for' per the language specification

*/
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
						tempToken = inputScanner.tokenScan();
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


/*
	RETURN STATEMENT

This function handles any return statements made in the input file program
We check for the 'return' token, and continue on, but if we don't find it, we give an error

*/
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












/*
	IDENTIFIER

This function handles setting the identifier on a given symbol and moving to the next token after

*/
DataStore Parser::Ident() {
	DataStore identifierData;
	identifierData.tempToken = tempToken;
	identifierData.tempType = tempToken.t_symbol->tempSymbolType;
	tempToken = inputScanner.tokenScan();
	return identifierData;
}


/*
	EXPRESSION

This function handles checking for 'not', as well as calling the Arithmetic and Expression Prime functions when appropriate
If we do find 'not', we type check that the following token resolves to an integer or boolean per the language specification
It also sets types and token members

*/
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


/*
	EXPRESSION PRIME
	
This function handles checking for 'and' and 'or' operators in the input file program
It will then call the Arithmetic function and recursively call itself and sets the types and token members from those calls
Finally, it checks the types of both sides of the 'and' or 'or' to ensure that they are comparisons of bool-to-bool or integer-to-integer per the language specification
If they do not align to this specification, we error

*/
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


/*
	ARITHMETIC

This function handles calling to the Relation function and the Arithmetic Prime function when appropriate
It also sets types and token members

*/
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


/*
	ARITHMETIC PRIME

This function handles checking for addition and subtraction operators in the input file program
It will then call the Relation function and recursively call itself and sets the types and token members from those calls
Finally, it checks that the types of both sides of the addition or subtraction operator align per the language specification, otherwise we error

*/
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


/*
	RELATION

This function handles calling to the Term function and the Relation Prime function when appropriate
It also sets types and token members

*/
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


/*
	RELATION PRIME

This function handles checking for any relational operators (==, <, !=, etc...) in the input file program
It will then call the Term function and recursively call itself and sets the types and token members from those calls
Finally, it checks that the types of both sides of the relational operator align per the language specification, otherwise we error

*/
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


/*
	TERM
	
This function handles calling the Factor function and the Term Prime function when appropriate
It also sets types and token members

*/
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


/*
	TERM PRIME

This function handles checking for multiplication and division operators in the input file program
It will then call the Factor function and recursively call itself and set the types and token members from those calls
Finally, it checks that the types of both sides of either the multiplication or division operator align per the language specification, otherwise we error

*/
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


/*
	FACTOR

This function handles the low-level parse tree operations for the input file program
This includes calling expression if we find a '(' so that it can interpret the inside of some statement or call, etc...,
Checking for '-' as a negative sign, and then checking for either an identifier or integer/float after it so that it can set the types,
Checking for an identifier, and setting it's type and token members,
Checking for a basic integer or float value and setting the types appropriately,
Checking for a basic string, character, or true/false, and setting those types and token members appropriately

*/
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


/*
	NAME

This function handles calling the Identifer function and setting the type and token members from that
It also checks for brackets, signalling that we are using an array accessor, which it then calls back to the Expression function to store
If we do find brackets, we check that the expression within them resolves to an integer per the language specification

*/
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
	}
	return nameData;
}


/*
	NUMBER

This function handles setting the token members for a number value

*/
DataStore Parser::Number() {
	DataStore numberData;
	numberData.tempToken = tempToken;
	tempToken = inputScanner.tokenScan();
	return numberData;
}
/*

This cpp file contains the definition for Parser class functions, each going through the LL1 parse tree for this language

*/

#include "Parser.h"
#include "Errors.h"

// Main parser constructor, which calls the constructor for the scanner instance
Parser::Parser(const char* filePath, SymTable& newSymbolTable) {
	inputScanner.init(filePath, newSymbolTable); // Call scanner constructor
	symbolTable = &newSymbolTable; // Create new symbol table
}

// Begin parsing the file by scanning the first token, and then running the program procedures
void Parser::parseFile() {
	tempToken = inputScanner.tokenScan(); // Get first token
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

	// CHECK FOR PROGRAM
	if (tempToken.t_type == PROGRAM) {
		ProgramHead();
		ProgramBody();

		symbolTable->CloseScope(); // Program finished, close the scope

		if (tempToken.t_type != FILEEND) {
			ParsingError tempError("FATAL ERROR, MISSING '.' FOR END OF FILE", -1, "");
			ResultOfParse.push_back(tempError);
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

	// CHECK FOR PROGRAM
	if (tempToken.t_type == PROGRAM) {
		symbolTable->OpenScope(); // Open first scope at program begin
		tempToken = inputScanner.tokenScan();
		Ident();

		// CHECK FOR IS
		if (tempToken.t_type == IS) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING 'IS' AT FILE BEGIN", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	return programHeadData;
}

// Program Body
DataStore Parser::ProgramBody() {

	DataStore programBodyData;

	// CHECK FOR VALID DECLARATION
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare(); // Run declaration procedure

		// Check that the token is SEMICOLON
		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ';' IN DECLARATION", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	// CHECK FOR BEGIN
	if (tempToken.t_type == BEGIN) {
		tempToken = inputScanner.tokenScan();
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING BEGIN IN DECLARATION", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}

	// CHECK FOR VALID STATEMENT
	while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
		Statement();

		// Check that the token is SEMICOLON
		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	// CHECK FOR END
	if (tempToken.t_type == END) {
		tempToken = inputScanner.tokenScan();

		// CHECK FOR PROGRAM
		if (tempToken.t_type == PROGRAM) {
			tempToken = inputScanner.tokenScan();
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
	bool isGlobal = false; // Bool for if declaration is going to be global


	// CHECK FOR GLOBAL
	if (tempToken.t_type == GLOBAL) {
		isGlobal = true; // Declaration of global identifier verified
		tempToken = inputScanner.tokenScan();

		// CHECK FOR PROCEDURE
		if (tempToken.t_type == PROCEDURE) {
			ProcDeclare(isGlobal);
		}

		// CHECK FOR VARIABLE
		else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
			VarDeclare(isGlobal);
		}
	}

	// CHECK FOR PROCEDURE
	else if (tempToken.t_type == PROCEDURE) {
		ProcDeclare(isGlobal);
	}

	// CHECK FOR VARIABLE
	else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
		VarDeclare(isGlobal);
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
	symbolTable->CloseScope(); // Procedure is over, close the scope
	return procedureDeclarationData;
}

// Variable Declaration
DataStore Parser::VarDeclare(bool isGlobal) {
	DataStore variableDeclarationData;
	bool negBound = false; // Bool for if an array bound is negative

	Symbol tempSymbol; // Create temporary symbol
	tempSymbol.isGlobal = isGlobal; // Set global flag
	DataStore dataToHandle = TypeMark(); // Get type
	tempSymbol.tempSymbolType = dataToHandle.tempType; // Set type back on temporary symbol

	Symbol* newSymbol = nullptr; // Initialize symbol to check against tables later
	dataToHandle = Ident(); // Get identifier
	variableDeclarationData.tempToken = dataToHandle.tempToken; // Set token
	tempSymbol.id = variableDeclarationData.tempToken.t_string; // Set id
	newSymbol = variableDeclarationData.tempToken.t_symbol; // Set new symbol

	// CHECK FOR LEFT BRACKET
	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan();

		// Check for a minus sign in bounds
		if (tempToken.t_type == SUB) {
			tempToken = inputScanner.tokenScan(); // Get number
			negBound = true; // Set flag for finding negative bound
		}
		dataToHandle = Number(); // Get number

		// Number must be int
		if (dataToHandle.tempToken.t_type != VALINT) {
			ParsingError tempError("PARSE ERROR, LOWER ARRAY BOUND MUST BE INT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
		else {
			variableDeclarationData.tempToken = dataToHandle.tempToken; // Set token

			// Check for negative bound
			if (negBound) {
				variableDeclarationData.tempToken.t_int = dataToHandle.tempToken.t_int * -1; // If negative bound, set int value to negative
			}
			tempSymbol.arrayLower = dataToHandle.tempToken.t_int; // No negative bound, set int
		}

		negBound = false; // If last bound was negative, need to reset

		// CHECK FOR COLON
		if (tempToken.t_type == COLON) {
			tempToken = inputScanner.tokenScan();

			// Check for minus sign in bounds
			if (tempToken.t_type == SUB) {
				tempToken = inputScanner.tokenScan(); // Get number
				negBound = true; // Set flag for finding negative bound
			}
			dataToHandle = Number(); // Get number

			// Number must be int
			if (dataToHandle.tempToken.t_int != VALINT) {
				ParsingError tempError("PARSE ERROR, UPPER ARRAY BOUND MUST BE INT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

			// Check that upper array bound is greater than lower array bound
			else {
				if (!(tempSymbol.arrayLower < dataToHandle.tempToken.t_int)) {
					ParsingError tempError("PARSE ERROR, UPPER ARRAY BOUND MUST BE GREATER THAN LOWER", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
				else {
					// Check for negative bound
					if (negBound) {
						variableDeclarationData.tempToken.t_int = dataToHandle.tempToken.t_int * -1; // If negative bound, set int value to negative
					}
					tempSymbol.arrayUpper = dataToHandle.tempToken.t_int; // No negative bound, set int
				}
			}

			// CHECK FOR RIGHT BRACKET
			if (tempToken.t_type == BRACKEND) {
				tempSymbol.isArray = true; // Officially an array, set symbol flag
				tempToken = inputScanner.tokenScan();
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING '[' IN ARRAY DECLARATION", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ':' IN ARRAY DECLARATION", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	// Check scopes
	if (tempSymbol.isGlobal == newSymbol->isGlobal) {

		// Verify that symbol has not been assigned, then set everything
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
			Symbol* checkGlobal = symbolTable->getSymbol(tempSymbol.id, true); // Search for identifier in global scope

			// Identifier not assigned, add to symbol table
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

	// CHECK FOR PROCEDURE
	if (tempToken.t_type == PROCEDURE) {
		tempToken = inputScanner.tokenScan();

		Symbol tempSymbol; // Create temporary symbol
		tempSymbol.isGlobal = isGlobal; // Set global flag
		tempSymbol.tempSymbolType = PROC; // Set symbol type to procedure

		Symbol* newSymbol = nullptr; // Initialize symbol to check against tables later
		DataStore dataToHandle = Ident(); // Get identifier
		procedureHeaderData.tempToken = dataToHandle.tempToken; // Set token
		tempSymbol.id = procedureHeaderData.tempToken.t_string; // Set id
		newSymbol = procedureHeaderData.tempToken.t_symbol; // Set new symbol

		symbolTable->OpenScope(); // Found procedure declaration, open scope

		// CHECK FOR LEFT PARENTHESES
		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();

			// CHECK FOR RIGHT PARENTHESES
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();
			}
			else {
				DataStore paramListData = ParamList(); // Get parameters
				tempSymbol.procedureParameters = paramListData.procedureParameters; // Set parameters

				// CHECK FOR RIGHT PARENTHESES
				if (tempToken.t_type == PARENEND) {
					tempToken = inputScanner.tokenScan();
				}
				else {
					ParsingError tempError("PARSE ERROR, MISSING ')' IN PROCEDURE HEADER", tempToken.lineNum, tempToken.t_string);
					ResultOfParse.push_back(tempError);
				}
			}

			// Check scopes
			if (tempSymbol.isGlobal == newSymbol->isGlobal) {

				// Verify that symbol has not been assigned, then set everything
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
					Symbol* checkGlobal = symbolTable->getSymbol(tempSymbol.id, true); // Search for identifier in global scope

					// Identifier not assigned, add to symbol table
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
		ParsingError tempError("PARSE ERROR, MISSING PROCEDURE HEADER", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return procedureHeaderData;
}

// Procedure Body
DataStore Parser::ProcBody() {

	DataStore procedureBodyData;

	// CHECK FOR VALID DECLARATION
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare();

		// CHECK FOR SEMICOLON
		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ';' IN DECLARATION", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	// CHECK FOR BEGIN
	if (tempToken.t_type == BEGIN) {
		tempToken = inputScanner.tokenScan();
	}
	else {
		ParsingError tempError("PARSE ERROR, MISSING BEGIN IN PROCEDURE BODY", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}

	// CHECK FOR VALID STATEMENT
	while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
		Statement();

		// CHECK FOR SEMICOLON
		if (tempToken.t_type == SEMICOLON) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}

	// CHECK FOR END
	if (tempToken.t_type == END) {
		tempToken = inputScanner.tokenScan();

		// CHECK FOR PROCEDURE
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

	DataStore dataFromParam = Param(); // Get parameter
	parameterListData.procedureParameters.push_back(dataFromParam.procedureParameters[0]); // Set parameter

	// CHECK FOR COMMA
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan();

		dataFromParam = ParamList(); // Get parameter list
		parameterListData.procedureParameters.insert(parameterListData.procedureParameters.end(), dataFromParam.procedureParameters.begin(), dataFromParam.procedureParameters.end()); // Set parameters
	}
	return parameterListData;
}

// Parameter
DataStore Parser::Param() {
	DataStore parameterData;

	Symbol* newSymbol = nullptr;
	DataStore dataFromVariableDeclaration = VarDeclare(false);
	newSymbol = dataFromVariableDeclaration.tempToken.t_symbol; // Set symbol

	// CHECK FOR IN
	if (tempToken.t_type == IN) {
		parameterData.procedureParameters.push_back({ newSymbol, INTYPE });
		tempToken = inputScanner.tokenScan();
	}

	// CHECK FOR OUT
	else if (tempToken.t_type == OUT) {
		parameterData.procedureParameters.push_back({ newSymbol, OUTTYPE });
		tempToken = inputScanner.tokenScan();
	}

	// CHECK FOR INOUT
	else if (tempToken.t_type == INOUT) {
		parameterData.procedureParameters.push_back({ newSymbol, INOUTTYPE });
		tempToken = inputScanner.tokenScan();
	}
	return parameterData;
}

// Type Mark
DataStore Parser::TypeMark() {

	DataStore typeData;

	// CHECK FOR INTEGER
	if (tempToken.t_type == INTEGER) {
		typeData.tempType = SYMINTEGER;
		tempToken = inputScanner.tokenScan();
	}

	// CHECK FOR FLOAT
	else if (tempToken.t_type == FLOAT) {
		typeData.tempType = SYMFLOAT;
		tempToken = inputScanner.tokenScan();
	}

	// CHECK FOR BOOL
	else if (tempToken.t_type == BOOL) {
		typeData.tempType = SYMBOOL;
		tempToken = inputScanner.tokenScan();
	}

	// CHECK FOR CHAR
	else if (tempToken.t_type == CHAR) {
		typeData.tempType = SYMCHAR;
		tempToken = inputScanner.tokenScan();
	}

	// CHECK FOR STRING
	else if (tempToken.t_type == STRING) {
		typeData.tempType = SYMSTRING;
		tempToken = inputScanner.tokenScan();
	}
	else {
		ParsingError tempError("PARSE ERROR, NO VALID TYPE DECLARED", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return typeData;
}

// Assign
DataStore Parser::Assign(bool onlyAssign) {

	DataStore assignData;

	// CHECK FOR IDENTIFIER
	if (tempToken.t_type == IDENTIFIER) {
		Ident();

		// CHECK FOR LEFT BRACKET
		if (tempToken.t_type == BRACKBEGIN) {
			tempToken = inputScanner.tokenScan();
			Expr();

			// CHECK FOR RIGHT BRACKET
			if (tempToken.t_type == BRACKEND) {
				tempToken = inputScanner.tokenScan();
				AssignState();
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ']' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}

		// CHECK FOR LEFT PARENTHESES
		else if (tempToken.t_type == PARENBEGIN && onlyAssign == false) {
			tempToken = inputScanner.tokenScan();
			ArgumentList();

			// CHECK FOR RIGHT PARENTHESES
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ')' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			if (tempToken.t_type == SEMIEQUAL) {
				AssignState();
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

	// CHECK FOR SEMIEQUALS
	if (tempToken.t_type == SEMIEQUAL) {
		tempToken = inputScanner.tokenScan();
		Expr();
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

	Expr();

	// CHECK FOR COMMA
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan();
		ArgumentList();
	}
	return argumentListData;
}

// If
DataStore Parser::If() {

	DataStore ifData;

	// CHECK FOR IF
	if (tempToken.t_type == IF) {
		tempToken = inputScanner.tokenScan();

		// CHECK FOR LEFT PARENTHESES
		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();
			Expr();

			// CHECK FOR RIGHT PARENTHESES
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();

				// CHECK FOR THEN
				if (tempToken.t_type == THEN) {
					tempToken = inputScanner.tokenScan();
					Statement();

					// CHECK FOR SEMICOLON
					if (tempToken.t_type == SEMICOLON) {
						tempToken = inputScanner.tokenScan();
					}
					else {
						ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}

					// CHECK FOR IDENTIFIER, IF, OR, or RETURN
					while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
						Statement();

						// CHECK FOR SEMICOLON
						if (tempToken.t_type == SEMICOLON) {
							tempToken = inputScanner.tokenScan();
						}
						else {
							ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
							ResultOfParse.push_back(tempError);
						}
					}

					// CHECK FOR ELSE
					if (tempToken.t_type == ELSE) {
						tempToken = inputScanner.tokenScan();
						Statement();

						// CHECK FOR SEMICOLON
						if (tempToken.t_type == SEMICOLON) {
							tempToken = inputScanner.tokenScan();
						}
						else {
							ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
							ResultOfParse.push_back(tempError);
						}

						// CHECK FOR IDENTIFIER, IF, FOR, or RETURN
						while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
							Statement();

							// CHECK FOR SEMICOLON
							if (tempToken.t_type == SEMICOLON) {
								tempToken = inputScanner.tokenScan();
							}
							else {
								ParsingError tempError("PARSE ERROR, MISSING ';' IN STATEMENT", tempToken.lineNum, tempToken.t_string);
								ResultOfParse.push_back(tempError);
							}
						}
					}

					// CHECK FOR END
					if (tempToken.t_type == END) {
						tempToken = inputScanner.tokenScan();

						// CHECK FOR IF
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
					ParsingError tempError("PARSE ERROR, MISSING THEN IN IF STATEMENT", tempToken.lineNum, tempToken.t_string);
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

	// CHECK FOR FOR
	if (tempToken.t_type == FOR) {
		tempToken = inputScanner.tokenScan();

		// CHECK FOR LEFT PARENTHESES
		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();
			Assign(true);

			// CHECK FOR SEMICOLON
			if (tempToken.t_type == SEMICOLON) {
				tempToken = inputScanner.tokenScan();
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ';' IN LOOP STATEMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
			Expr();

			// CHECK FOR RIGHT PARENTHESES
			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();

				// CHECK FOR IDENTIFIER, IF, FOR or RETURN
				while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
					Statement(); // Run statement procedure

					// CHECK FOR SEMICOLON
					if (tempToken.t_type == SEMICOLON) {
						tempToken = inputScanner.tokenScan();
					}
					else {
						ParsingError tempError("PARSE ERROR, MISSING ';' IN LOOP STATEMENT", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}
				}

				// CHECK FOR END
				if (tempToken.t_type == END) {
					tempToken = inputScanner.tokenScan();

					// CHECK FOR FOR
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

	// CHECK FOR RETURN
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
	identifierData.tempToken = tempToken; // Set token
	tempToken = inputScanner.tokenScan();
	return identifierData;
}

// Expression
DataStore Parser::Expr() {

	DataStore expressionData;

	// CHECK FOR NOT
	if (tempToken.t_type == NOT) {
		tempToken = inputScanner.tokenScan();
	}
	Arith();
	ExprPrime();
	return expressionData;
}

// Expression Prime
DataStore Parser::ExprPrime() {

	DataStore expressionData;

	// CHECK FOR AND
	if (tempToken.t_type == AND) {
		tempToken = inputScanner.tokenScan();
		Arith();
		ExprPrime();
		return expressionData;
	}

	// CHECK FOR OR
	else if (tempToken.t_type == OR) {
		tempToken = inputScanner.tokenScan();
		Arith();
		ExprPrime();
		return expressionData;
	}
	else {
		return expressionData;
	}
}

// Arithmetic
DataStore Parser::Arith() {
	DataStore arithmeticData;
	Relat();
	ArithPrime();
	return arithmeticData;
}

// Arithmetic Prime
DataStore Parser::ArithPrime() {

	DataStore arithmeticData;

	// CHECK FOR ADD
	if (tempToken.t_type == ADD) {
		tempToken = inputScanner.tokenScan();
		Relat();
		ArithPrime();
		return arithmeticData;
	}

	// CHECK FOR SUB
	else if (tempToken.t_type == SUB) {
		tempToken = inputScanner.tokenScan();
		Relat();
		ArithPrime();
		return arithmeticData;
	}
	else {
		return arithmeticData;
	}
}

// Relation
DataStore Parser::Relat() {
	DataStore relationData;
	Term();
	RelatPrime();
	return relationData;
}

// Relation Prime
DataStore Parser::RelatPrime() {
	DataStore relationData;

	// CHECK FOR LESSER THAN
	if (tempToken.t_type == LESS) {
		tempToken = inputScanner.tokenScan();
		Term();
		RelatPrime();
		return relationData;
	}

	// CHECK FOR GREATER THAN
	else if (tempToken.t_type == GREAT) {
		tempToken = inputScanner.tokenScan();
		Term();
		RelatPrime();
		return relationData;
	}

	// CHECK FOR DOUBLE EQUALS
	else if (tempToken.t_type == DOUBLEEQUAL) {
		tempToken = inputScanner.tokenScan();
		Term();
		RelatPrime();
		return relationData;
	}

	// CHECK FOR NOT EQUALS
	else if (tempToken.t_type == NOTEQUAL) {
		tempToken = inputScanner.tokenScan();
		Term();
		RelatPrime();
		return relationData;
	}

	// CHECK FOR LESSER THAN OR EQUAL TO
	else if (tempToken.t_type == LESSEQ) {
		tempToken = inputScanner.tokenScan();
		Term();
		RelatPrime();
		return relationData;
	}

	// CHECK FOR GREATER THAN OR EQUAL TO
	else if (tempToken.t_type == GREATEQ) {
		tempToken = inputScanner.tokenScan();
		Term();
		RelatPrime();
		return relationData;
	}
	else {
		return relationData;
	}
}

// Term
DataStore Parser::Term() {
	DataStore termData;
	Factor();
	TermPrime();
	return termData;
}

// Term Prime
DataStore Parser::TermPrime() {
	DataStore termData;

	// CHECK FOR MULTIPLICATION
	if (tempToken.t_type == MULT) {
		tempToken = inputScanner.tokenScan();
		Factor();
		TermPrime();
		return termData;
	}

	// CHECK FOR DIVISION
	if (tempToken.t_type == DIVIDE) {
		tempToken = inputScanner.tokenScan();
		Factor();
		TermPrime();
		return termData;
	}
	else {
		return termData;
	}
}

// Factor
DataStore Parser::Factor() {
	DataStore factorData;

	// CHECK FOR LEFT PARENTHESES
	if (tempToken.t_type == PARENBEGIN) {
		tempToken = inputScanner.tokenScan();
		Expr();

		// CHECK FOR RIGHT PARENTHESES
		if (tempToken.t_type == PARENEND) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ')' IN FACTOR", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
		return factorData;
	}

	// CHECK FOR SUBTRACT
	else if (tempToken.t_type == SUB) {
		tempToken = inputScanner.tokenScan();

		// CHECK FOR IDENTIFIER
		if (tempToken.t_type == IDENTIFIER) {
			Name();
			return factorData;
		}

		// CHECK FOR FLOAT OR INTEGER
		if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
			Number();
			return factorData;
		}
		return factorData;
	}

	// CHECK FOR IDENTIFIER
	else if (tempToken.t_type == IDENTIFIER) {
		Name();
		return factorData;
	}

	// CHECK FOR FLOAT OR INTEGER
	else if (tempToken.t_type == VALFLOAT || tempToken.t_type == VALINT) {
		Number();
		return factorData;
	}

	// CHECK FOR STRING
	else if (tempToken.t_type == VALSTRING) {
		tempToken = inputScanner.tokenScan();
		return factorData;
	}

	// CHECK FOR CHAR
	else if (tempToken.t_type == VALCHAR) {
		tempToken = inputScanner.tokenScan();
		return factorData;
	}

	// CHECK FOR TRUE
	else if (tempToken.t_type == TRUE) {
		tempToken = inputScanner.tokenScan();
		return factorData;
	}

	// CHECK FOR FALSE
	else if (tempToken.t_type == FALSE) {
		tempToken = inputScanner.tokenScan();
		return factorData;
	}
	else {
		return factorData;
	}
}

// Name
DataStore Parser::Name() {
	DataStore nameData;
	Ident();

	// CHECK FOR LEFT BRACKET
	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan();
		Expr();

		// CHECK FOR RIGHT BRACKET
		if (tempToken.t_type == BRACKEND) {
			tempToken = inputScanner.tokenScan();
		}
		else {
			ParsingError tempError("PARSE ERROR, MISSING ']' IN NAME", tempToken.lineNum, tempToken.t_string);
			ResultOfParse.push_back(tempError);
		}
	}
	return nameData;
}

// Number
DataStore Parser::Number() {
	DataStore numberData;
	numberData.tempToken = tempToken;
	tempToken = inputScanner.tokenScan();
	return numberData;
}
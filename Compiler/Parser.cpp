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
		Ident();

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

	// Declarations
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare();

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
		ParsingError tempError("PARSE ERROR, MISSING BEGIN IN DECLARATION", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}

	// Statements
	while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
		Statement();

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
	bool isGlobal = false;

	// Global declarations
	if (tempToken.t_type == GLOBAL) {
		isGlobal = true;
		tempToken = inputScanner.tokenScan();

		// Procedure declaration
		if (tempToken.t_type == PROCEDURE) {
			ProcDeclare(isGlobal);
		}

		// Variable declaration
		else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
			VarDeclare(isGlobal);
		}
	}

	// Local procedure declaration
	else if (tempToken.t_type == PROCEDURE) {
		ProcDeclare(isGlobal);
	}

	// Local variable declaration
	else if (tempToken.t_type == INTEGER || tempToken.t_type == CHAR || tempToken.t_type == STRING || tempToken.t_type == FLOAT || tempToken.t_type == BOOL) {
		VarDeclare(isGlobal);
	}
	return declarationData;
}

// Statement
DataStore Parser::Statement() {

	DataStore statementData;

	if (tempToken.t_type == IDENTIFIER) {
		Assign(false);
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

	if (tempToken.t_type == BRACKBEGIN) {
		tempToken = inputScanner.tokenScan();

		// Check for negative bound
		if (tempToken.t_type == SUB) {
			tempToken = inputScanner.tokenScan();
			negBound = true;
		}
		dataToHandle = Number();
		
		if (dataToHandle.success) {
			if (dataToHandle.tempToken.t_type != VALINT) {
				ParsingError tempError("PARSE ERROR, LOWER ARRAY BOUND MUST BE INT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
			else {
				variableDeclarationData.tempToken = dataToHandle.tempToken;

				// If the bound is negative, reflect that in the token and the array bound for the symbol
				if (negBound) {
					variableDeclarationData.tempToken.t_int = dataToHandle.tempToken.t_int * -1;
				}
				tempSymbol.arrayLower = dataToHandle.tempToken.t_int;
			}
		}

		if (tempToken.t_type == COLON) {
			tempToken = inputScanner.tokenScan();

			// Check for negative bound
			if (tempToken.t_type == SUB) {
				tempToken = inputScanner.tokenScan();
				negBound = true;
			}
			dataToHandle = Number();

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

						// If the bound is negative, reflect that in the token and the array bound for the symbol
						if (negBound) {
							variableDeclarationData.tempToken.t_int = dataToHandle.tempToken.t_int * -1;
						}
						tempSymbol.arrayUpper = dataToHandle.tempToken.t_int;
					}
				}
			}

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

	if (tempSymbol.isGlobal == returnedSymbol->isGlobal) {
		if (returnedSymbol->tempSymbolType == UNASSIGNED) {
			returnedSymbol->tempSymbolType = tempSymbol.tempSymbolType;
			returnedSymbol->isArray = tempSymbol.isArray;
			returnedSymbol->arrayLower = tempSymbol.arrayLower;
			returnedSymbol->arrayUpper = tempSymbol.arrayUpper;
		}
		else {
			ParsingError tempError("PARSE ERROR, REDECLARING LOCAL VARIABLE", tempToken.lineNum, tempToken.t_string);
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

	if (tempToken.t_type == PROCEDURE) {
		tempToken = inputScanner.tokenScan();

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

		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();


			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();
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
					ParsingError tempError("PARSE ERROR, REDECLARING LOCAL VARIABLE", tempToken.lineNum, tempToken.t_string);
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

	// Declarations
	while (tempToken.t_type == GLOBAL || tempToken.t_type == PROCEDURE || tempToken.t_type == INTEGER || tempToken.t_type == FLOAT || tempToken.t_type == BOOL || tempToken.t_type == STRING || tempToken.t_type == CHAR) {
		Declare();

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
		Statement();

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

	DataStore dataFromParam = Param();
	if (dataFromParam.success) {
		parameterListData.procedureParameters.push_back(dataFromParam.procedureParameters[0]);
	}

	// Multiple parameters
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan();

		dataFromParam = ParamList();
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
	DataStore dataFromVariableDeclaration = VarDeclare(false);
	if (dataFromVariableDeclaration.success) {
		returnedSymbol = dataFromVariableDeclaration.tempToken.t_symbol;
	}

	if (tempToken.t_type == IN) {
		parameterData.procedureParameters.push_back({ returnedSymbol, INTYPE });
		tempToken = inputScanner.tokenScan();
	}

	else if (tempToken.t_type == OUT) {
		parameterData.procedureParameters.push_back({ returnedSymbol, OUTTYPE });
		tempToken = inputScanner.tokenScan();
	}

	else if (tempToken.t_type == INOUT) {
		parameterData.procedureParameters.push_back({ returnedSymbol, INOUTTYPE });
		tempToken = inputScanner.tokenScan();
	}
	return parameterData;
}

// Type Mark
DataStore Parser::TypeMark() {

	DataStore typeData;

	if (tempToken.t_type == INTEGER) {
		typeData.tempType = SYMINTEGER;
		tempToken = inputScanner.tokenScan();
	}
	else if (tempToken.t_type == FLOAT) {
		typeData.tempType = SYMFLOAT;
		tempToken = inputScanner.tokenScan();
	}
	else if (tempToken.t_type == BOOL) {
		typeData.tempType = SYMBOOL;
		tempToken = inputScanner.tokenScan();
	}
	else if (tempToken.t_type == CHAR) {
		typeData.tempType = SYMCHAR;
		tempToken = inputScanner.tokenScan();
	}
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
	DataStore destData;

	if (tempToken.t_type == IDENTIFIER) {
		DataStore dataToHandle = Ident();
		if (dataToHandle.success) {
			assignData.tempToken = dataToHandle.tempToken;
			assignData.tempType = dataToHandle.tempType;
			destData = dataToHandle;
		}

		if (tempToken.t_type == BRACKBEGIN) {
			tempToken = inputScanner.tokenScan();
			dataToHandle = Expr();

			if (dataToHandle.tempType != SYMINTEGER) {
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

		else if (tempToken.t_type == PARENBEGIN && onlyAssign == false) {
			tempToken = inputScanner.tokenScan();
			destData = ArgumentList();

			if (destData.success) {
				destData.args = destData.args;
			}

			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();
			}
			else {
				ParsingError tempError("PARSE ERROR, MISSING ')' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}

			if (destData.tempToken.t_symbol->procedureParameters.size() == destData.args.size()) {

				// Check parameter types
				for (int i = 0; i < destData.procedureParameters.size(); i++) {
					if (destData.tempToken.t_symbol->procedureParameters[i].first->tempSymbolType == destData.args[i]) {

					}
					else {
						ParsingError tempError("PARSE ERROR, MISMATCHED PROCEDURE PARAMETER TYPES", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}
				}
			}
			else if (destData.tempToken.t_symbol->procedureParameters.size() > destData.args.size()) {
				ParsingError tempError("PARSE ERROR, TOO FEW ARGUMENTS IN PROCEDURE CALL", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
			else {
				ParsingError tempError("PARSE ERROR, TOO MANY ARGUMENTS IN PROCEDURE CALL", tempToken.lineNum, tempToken.t_string);
				ResultOfParse.push_back(tempError);
			}
		}
		else {
			if (tempToken.t_type == SEMIEQUAL) {
				AssignState(destData.tempToken, destData.tempType);
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

	if (tempToken.t_type == SEMIEQUAL) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = Expr();
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
		ParsingError tempError("PARSE ERROR, MISSING ':=' IN ASSIGNMENT", tempToken.lineNum, tempToken.t_string);
		ResultOfParse.push_back(tempError);
	}
	return assignStateData;
}

// Argument List
DataStore Parser::ArgumentList() {

	DataStore argumentListData;
	DataStore dataToHandle = Expr();
	if (dataToHandle.success) {
		dataToHandle.args.push_back(dataToHandle.tempType);
	}

	// Multiple arguments
	if (tempToken.t_type == COMMA) {
		tempToken = inputScanner.tokenScan();
		dataToHandle = ArgumentList();
		if (dataToHandle.success) {
			dataToHandle.args.insert(dataToHandle.args.end(), dataToHandle.args.begin(), dataToHandle.args.end());
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
			DataStore dataToHandle = Expr();
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

			if (tempToken.t_type == PARENEND) {
				tempToken = inputScanner.tokenScan();

				if (tempToken.t_type == THEN) {
					tempToken = inputScanner.tokenScan();
					Statement();

					if (tempToken.t_type == SEMICOLON) {
						tempToken = inputScanner.tokenScan();
					}
					else {
						ParsingError tempError("PARSE ERROR, MISSING ';' AFTER THEN IN STATEMENT", tempToken.lineNum, tempToken.t_string);
						ResultOfParse.push_back(tempError);
					}

					while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
						Statement();

						if (tempToken.t_type == SEMICOLON) {
							tempToken = inputScanner.tokenScan();
						}
						else {
							ParsingError tempError("PARSE ERROR, MISSING ';' AFTER IDENT/IF/FOR/RETURN IN STATEMENT", tempToken.lineNum, tempToken.t_string);
							ResultOfParse.push_back(tempError);
						}
					}

					if (tempToken.t_type == ELSE) {
						tempToken = inputScanner.tokenScan();
						Statement();

						if (tempToken.t_type == SEMICOLON) {
							tempToken = inputScanner.tokenScan();
						}
						else {
							ParsingError tempError("PARSE ERROR, MISSING ';' AFTER ELSE IN STATEMENT", tempToken.lineNum, tempToken.t_string);
							ResultOfParse.push_back(tempError);
						}

						while (tempToken.t_type == IDENTIFIER || tempToken.t_type == IF || tempToken.t_type == FOR || tempToken.t_type == RETURN) {
							Statement();

							if (tempToken.t_type == SEMICOLON) {
								tempToken = inputScanner.tokenScan();
							}
							else {
								ParsingError tempError("PARSE ERROR, MISSING ';' AFTER IDENT/IF/FOR/RETURN IN STATEMENT", tempToken.lineNum, tempToken.t_string);
								ResultOfParse.push_back(tempError);
							}
						}
					}

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

	if (tempToken.t_type == FOR) {
		tempToken = inputScanner.tokenScan();

		if (tempToken.t_type == PARENBEGIN) {
			tempToken = inputScanner.tokenScan();
			Assign(true);

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
					Statement();

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

	// Type check
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
		tempToken = inputScanner.tokenScan();
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
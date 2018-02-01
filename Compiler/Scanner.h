#pragma once

#include "Token.h"

#include <vector>

class Scanner {
public:
	void scanIn (const char* filePath);
	vector<token> getTokens();

private:
	void checkForReserves (token &tempToken);
	token tokenScan (FILE* stream);
	vector<token> tempTokenList;
	FILE* tempStream;
};
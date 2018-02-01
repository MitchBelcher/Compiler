#pragma once

#include "Token.h"

#include <vector>

class Scanner {
public:
	void init (const char* filePath);
	~Scanner();

	//void scanIn (const char* filePath);
	token tokenScan ();
	vector<token> getTokens();

private:
	void checkForReserves (token &tempToken);
	//token tokenScan (FILE* stream);
	vector<token> tempTokenList;
	FILE* tempStream;
};
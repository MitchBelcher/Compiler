#pragma once

#include <fstream>

using namespace std;

class CodeGen {
public:
	void EndProgram();
	void BeginMainFunc();

private:
	ofstream outputSource;

};
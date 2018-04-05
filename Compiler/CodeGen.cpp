#include "CodeGen.h"

using namespace std;

void CodeGen::BeginMainFunc() {
	outputSource << "int main() {\n";
}

void CodeGen::EndProgram() {
	outputSource << "\n\treturn 0;\n}";
}
#include <iostream>

#include "IO/File.h"
#include "Compiler/Parser.h"

void PrintResult(Parser &parser, const CompilerResult &result) {
	switch (result.Type) {
		case ResultType::Success:
			std::cout << "Success" << std::endl;
			parser.PrintProgramTree();
			break;
		case ResultType::InvalidToken:
			std::cout << "Invalid Token" << std::endl;
			break;
		case ResultType::InvalidSyntax:
			std::cout << "Invalid Syntax" << std::endl;
			break;
		case ResultType::Failure:
			std::cout << "Internal Compiler Error" << std::endl;
			break;
	}
}

int main(int argc, char** argv) {

	std::string code = File::ReadTextFile("../example.csl");

	Lexer lexer(code);
    Parser parser(lexer);
	CompilerResult result = parser.Parse();

	PrintResult(parser, result);

	return 0;
}




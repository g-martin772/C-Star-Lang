#include <iostream>

#include "IO/File.h"
#include "Compiler/Lexer.h"

int main(int argc, char** argv) {

	std::string code = File::ReadTextFile("../example.csl");

	Lexer lexer(code);

	Token token;
	do {
		token = lexer.GetNextToken();
		std::cout << "Token Type: " << static_cast<int>(token.Type) << ", Content: " << token.Content << std::endl;
	} while (token.Type != TokenType::INVALID);

    return 0;
}
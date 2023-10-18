#include <iostream>

#include "IO/File.h"
#include "Compiler/Parser.h"

int main(int argc, char** argv) {

	std::string code = File::ReadTextFile("../example.csl");

	Lexer lexer(code);
    Parser parser;

	Token token;
	do {
		token = lexer.GetNextToken();
		std::cout << "Token Type: " << static_cast<int>(token.Type) << ", Content: " << token.Content << std::endl;
        CompilerResult result = parser.ConsumeToken(token);
        switch (result.Type) {
            case ResultType::Success:
                continue;
            case ResultType::InvalidToken:
                std::cout << "Invalid Token: " << static_cast<int>(token.Type) << ", Content: " << token.Content << std::endl;
                break;
            case ResultType::InvalidSyntax:
                std::cout << "Invalid Syntax at " << static_cast<int>(token.Type) << " Token, Content: " << token.Content << std::endl;
                break;
            case ResultType::Failure:
                std::cout << "Internal Compiler Error while Trying to parse Token: " << static_cast<int>(token.Type) << ", Content: " << token.Content << std::endl;
                break;
        }
	} while (token.Type != TokenType::INVALID);

    return 0;
}
#pragma once

#include <string>
#include <utility>

enum class TokenType {
	INVALID,
	IDENTIFIER,
	INT_LIT,
	STRING_LIT,
	SEMICOLON
};

struct Token {
	TokenType Type = TokenType::INVALID;
	std::string Content;
};

class Lexer {
public:
	explicit Lexer(std::string  input) : m_Input(std::move(input)), m_Position(0) {}

	Token GetNextToken();
private:
	std::string m_Input;
	size_t m_Position;

	void SkipWhitespace();
	std::string GetIdentifier();
	std::string GetNumber();
	std::string GetString();
};

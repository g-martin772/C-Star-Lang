#pragma once

#include <string>
#include <utility>

enum class TokenType {
	Invalid = 0,
	Identifier,
	IntLit,
	StringLit,
	Semi,
    CurlyOpen,
    CurlyClose,
    ParenOpen,
    ParenClose,
    SquareOpen,
    SquareClose,
    Comma,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Equal,
    Greater,
    Less,
    Exclamation,
    Ampersand,
    Pipe
};

struct Token {
	TokenType Type = TokenType::Invalid;
	std::string Content;
};

class Lexer {
public:
	explicit Lexer(std::string  input) : m_Input(std::move(input)), m_Position(0) {}

	Token Consume();
	Token Peek(int offset = 1);
	Token At(int pos, bool consume);
    static std::string TokenTypeToString(TokenType type);
private:
	std::string m_Input;
	size_t m_Position;

	int SkipWhitespace();
	std::string GetIdentifier();
	std::string GetNumber();
	std::string GetString();
};

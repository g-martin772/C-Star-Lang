#include <iostream>
#include "Lexer.h"

Token Lexer::Consume() {
	Token t = At(m_Position);
	m_Position++;
	std::cout << "Token Type: " << Lexer::TokenTypeToString(t.Type) << ", Content: " << t.Content << std::endl;
	return t;
}

Token Lexer::Peek(int offset) {
	return At(m_Position + offset);
}

void Lexer::SkipWhitespace() {
	while (m_Position < m_Input.length() && std::isspace(m_Input[m_Position])) {
		m_Position++;
	}
}

std::string Lexer::GetIdentifier() {
	size_t start = m_Position;
	while (m_Position < m_Input.length() && (std::isalpha(m_Input[m_Position]) || isdigit(m_Input[m_Position]))) {
		m_Position++;
	}
	return m_Input.substr(start, m_Position - start);
}

std::string Lexer::GetNumber() {
	size_t start = m_Position;
	while (m_Position < m_Input.length() && isdigit(m_Input[m_Position])) {
		m_Position++;
	}
	return m_Input.substr(start, m_Position - start);
}

std::string Lexer::GetString() {
	size_t start = m_Position + 1;
	m_Position++;
	while (m_Position < m_Input.length() && m_Input[m_Position] != '"') {
		m_Position++;
	}
	if (m_Position < m_Input.length() && m_Input[m_Position] == '"') {
		m_Position++;
	}
	return m_Input.substr(start, m_Position - start - 1);
}

std::string Lexer::TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Invalid: return "Invalid";
        case TokenType::Identifier: return "Identifier";
        case TokenType::IntLit: return "IntLit";
        case TokenType::StringLit: return "StringLit";
        case TokenType::Semi: return "Semi";
        case TokenType::CurlyOpen: return "CurlyOpen";
        case TokenType::CurlyClose: return "CurlyClose";
        case TokenType::ParenOpen: return "ParenOpen";
        case TokenType::ParenClose: return "ParenClose";
        case TokenType::SquareOpen: return "SquareOpen";
        case TokenType::SquareClose: return "SquareClose";
        case TokenType::Comma: return "Comma";
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Star: return "Star";
        case TokenType::Slash: return "Slash";
        case TokenType::Percent: return "Percent";
        case TokenType::Equal: return "Equal";
        case TokenType::Greater: return "Greater";
        case TokenType::Less: return "Less";
        case TokenType::Exclamation: return "Exclamation";
        case TokenType::Ampersand: return "Ampersand";
        case TokenType::Pipe: return "Pipe";
        default: return "Unknown";
    }
}

Token Lexer::At(int pos) {
	Token token;

	acquireChar:

	if (pos >= m_Input.length()) {
		token.Type = TokenType::Invalid;
		return token;
	}

	char currentChar = m_Input[pos];

	if (std::isspace(currentChar)) {
		SkipWhitespace();
		goto acquireChar;
	}

	if (std::isalpha(currentChar)) {
		token.Type = TokenType::Identifier;
		token.Content = GetIdentifier();
	} else if (isdigit(currentChar)) {
		token.Type = TokenType::IntLit;
		token.Content = GetNumber();
	} else if (currentChar == '"') {
		token.Type = TokenType::StringLit;
		token.Content = GetString();
	} else if (currentChar == ';') {
		token.Type = TokenType::Semi;
		token.Content = ";";
	} else if (currentChar == '{') {
		token.Type = TokenType::CurlyOpen;
		token.Content = "{";
	} else if (currentChar == '}') {
		token.Type = TokenType::CurlyClose;
		token.Content = "}";
	} else if (currentChar == '(') {
		token.Type = TokenType::ParenOpen;
		token.Content = "(";
	} else if (currentChar == ')') {
		token.Type = TokenType::ParenClose;
		token.Content = ")";
	} else if (currentChar == '[') {
		token.Type = TokenType::SquareOpen;
		token.Content = "[";
	} else if (currentChar == ']') {
		token.Type = TokenType::SquareClose;
		token.Content = "]";
	} else if (currentChar == ',') {
		token.Type = TokenType::Comma;
		token.Content = ",";
	} else if (currentChar == '+') {
		token.Type = TokenType::Plus;
		token.Content = "+";
	} else if (currentChar == '-') {
		token.Type = TokenType::Minus;
		token.Content = "-";
	} else if (currentChar == '*') {
		token.Type = TokenType::Star;
		token.Content = "*";
	} else if (currentChar == '/') {
		token.Type = TokenType::Slash;
		token.Content = "/";
	} else if (currentChar == '%') {
		token.Type = TokenType::Percent;
		token.Content = "%";
	} else if (currentChar == '=') {
		token.Type = TokenType::Equal;
		token.Content = "=";
	} else if (currentChar == '>') {
		token.Type = TokenType::Greater;
		token.Content = ">";
	} else if (currentChar == '<') {
		token.Type = TokenType::Less;
		token.Content = "<";
	} else if (currentChar == '!') {
		token.Type = TokenType::Exclamation;
		token.Content = "!";
	} else if (currentChar == '&') {
		token.Type = TokenType::Ampersand;
		token.Content = "&";
	} else if (currentChar == '|') {
		token.Type = TokenType::Pipe;
		token.Content = "|";
	}

	return token;
}

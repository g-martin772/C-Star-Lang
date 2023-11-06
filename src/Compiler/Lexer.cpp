#include <iostream>
#include "Lexer.h"

Token Lexer::Consume() {
	Token t = At(m_Position, true);
	m_Position++;
	std::cout << "Token Type: " << Lexer::TokenTypeToString(t.Type) << ", Content: " << t.Content << std::endl;
	return t;
}

Token Lexer::Peek(int offset) {
	Token t = At(m_Position + offset, false);
	return t;
}

int Lexer::SkipWhitespace() {
	size_t pos = m_Position;
	while (pos < m_Input.length() && std::isspace(m_Input[pos])) {
		pos++;
	}
	return pos - m_Position;
}

std::string Lexer::GetIdentifier() {
	size_t start = m_Position, pos = m_Position;
	while (pos < m_Input.length() && (std::isalpha(m_Input[pos]) || isdigit(m_Input[pos]))) {
		pos++;
	}
	return m_Input.substr(start, pos - start);
}

std::string Lexer::GetNumber() {
	size_t start = m_Position, pos = m_Position;
	while (pos < m_Input.length() && isdigit(m_Input[pos])) {
		pos++;
	}
	return m_Input.substr(start, pos - start);
}

std::string Lexer::GetString() {
	size_t start = m_Position + 1, pos = m_Position + 1;
	while (pos < m_Input.length() && m_Input[pos] != '"') {
		pos++;
	}
	if (pos < m_Input.length() && m_Input[pos] == '"') {
		pos++;
	}
	return m_Input.substr(start, pos - start - 1);
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

Token Lexer::At(int pos, bool consume) {
	Token token;

	if (pos >= m_Input.length()) {
		token.Type = TokenType::Invalid;
		return token;
	}

	pos += SkipWhitespace();
	char currentChar = m_Input[pos];

	if (std::isalpha(currentChar)) {
		token.Type = TokenType::Identifier;
		token.Content = GetIdentifier();
		m_Position += token.Content.length() * consume;
	} else if (isdigit(currentChar)) {
		token.Type = TokenType::IntLit;
		token.Content = GetNumber();
		m_Position += token.Content.length() * consume;
	} else if (currentChar == '"') {
		token.Type = TokenType::StringLit;
		token.Content = GetString();
		m_Position += (1 +token.Content.length()) * consume;
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

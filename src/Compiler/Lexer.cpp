#include "Lexer.h"

Token Lexer::GetNextToken() {
	Token token;
	if (m_Position >= m_Input.length()) {
		token.Type = TokenType::INVALID;
		return token;
	}

	char currentChar = m_Input[m_Position];

	if (std::isspace(currentChar)) {
		SkipWhitespace();
		return GetNextToken();
	}

	if (std::isalpha(currentChar)) {
		token.Type = TokenType::IDENTIFIER;
		token.Content = GetIdentifier();
	} else if (isdigit(currentChar)) {
		token.Type = TokenType::INT_LIT;
		token.Content = GetNumber();
	} else if (currentChar == '"') {
		token.Type = TokenType::STRING_LIT;
		token.Content = GetString();
	} else if (currentChar == ';') {
		token.Type = TokenType::SEMICOLON;
		token.Content = ";";
		m_Position++;
	} else {
		m_Position++;
	}

	return token;
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

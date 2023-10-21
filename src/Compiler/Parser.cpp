#include <algorithm>
#include <iostream>
#include "Parser.h"

static const std::vector<std::string> s_Reserved {
	"true",
	"false",
	"return",
	"if",
	"while",
	"for",
	"do",
	"when",
	"as",
	"null",
	"class",
	"struct",
	"this",
	"base",
	"new",
	"switch",
	"case",
	"break",
	"continue"
};

static const std::vector<std::string> s_DataTypes = {
		"void",
		"int",
		"bool",
		"char"
};

Parser::Parser(Lexer& lexer) : m_Lexer(lexer) {

}

CompilerResult Parser::Parse() {
	bool parsing = true;
	while(parsing) {
		Token token = m_Lexer.GetNextToken();

		if(token.Type == TokenType::Invalid) {
			parsing = false;
			continue;
		}

		if(m_CurrentFunction == nullptr && token.Type == TokenType::Identifier) {
			m_CurrentFunction = new FunctionNode();

			// Type
			if(!IsDataType(token.Content))
				return ResultType::InvalidSyntax;
			m_CurrentFunction->ReturnType = token.Content;

			// Name
			token = m_Lexer.GetNextToken();
			if(token.Type != TokenType::Identifier)
				return ResultType::InvalidSyntax;
			if(IsReserved(token.Content))
				return ResultType::InvalidSyntax;
			m_CurrentFunction->Name = token.Content;

			// Parameters
			token = m_Lexer.GetNextToken();
			if(token.Type != TokenType::ParenOpen)
				return ResultType::InvalidSyntax;
			while (true) {
				token = m_Lexer.GetNextToken();
				if(token.Type != TokenType::Identifier || !IsDataType(token.Content))
					return ResultType::InvalidSyntax;
				m_CurrentFunction->ParameterTypes.push_back(token.Content);
				token = m_Lexer.GetNextToken();
				if(token.Type != TokenType::Identifier || IsDataType(token.Content) || IsReserved(token.Content))
					return ResultType::InvalidSyntax;
				m_CurrentFunction->Parameters.push_back(token.Content);
				token = m_Lexer.GetNextToken();
				if(token.Type == TokenType::Comma)
					continue;
				if(token.Type == TokenType::ParenClose)
					break;
				return ResultType::InvalidSyntax;
			}

			// Block Open
			token = m_Lexer.GetNextToken();
			if(token.Type != TokenType::CurlyOpen)
				return ResultType::InvalidSyntax;
			token = m_Lexer.GetNextToken();
		}

		// TODO: Parse Expressions

		if(token.Type == TokenType::CurlyClose) {
			m_ProgramNode.Functions.push_back(m_CurrentFunction);
			m_CurrentFunction = nullptr;
		}

	}


	return ResultType::Success;
}

bool Parser::IsReserved(const std::string& t) {
	if(t.empty())
		return false;

	if(std::find(s_Reserved.begin(), s_Reserved.end(), t) != s_Reserved.end())
		return true;
	return false;
}

bool Parser::IsDataType(const std::string &t) {
	if(t.empty())
		return false;

	if(std::find(s_DataTypes.begin(), s_DataTypes.end(), t) != s_DataTypes.end())
		return true;
	return false;
}

void Parser::PrintProgramTree() {
	for(FunctionNode* function : m_ProgramNode.Functions) {
		std::cout << "  ";

		std::cout << "Function: " << function->Name << std::endl;
	}
}

ProgramNode::~ProgramNode() {
	for(auto& fn : Functions)
		delete fn;
}

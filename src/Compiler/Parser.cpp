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
			m_CurrentBlock = &m_CurrentFunction->Block;
		}

		// Block Open
		if(token.Type == TokenType::CurlyOpen) {
			BlockExpression* newExpression = new BlockExpression(m_CurrentBlock);
			m_CurrentBlock->Expressions.push_back({
				ExpressionType::Block,
				newExpression
			});
			m_CurrentBlock = newExpression;
			continue;
		}

		// Block Close
		if(token.Type == TokenType::CurlyClose) {
			if(m_CurrentBlock == &m_CurrentFunction->Block) {
				m_ProgramNode.Functions.push_back(m_CurrentFunction);
				m_CurrentFunction = nullptr;
				m_CurrentBlock = nullptr;
				continue;
			} else {
				m_CurrentBlock = m_CurrentBlock->Parent;
				continue;
			}
		}

		// Parse Expression
		if(token.Type == TokenType::Identifier && IsDataType(token.Content)) {
			std::string type = token.Content;

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

void PrintBlockExpression(BlockExpression* expression, int indent) {
	for(int i = 0; i < indent; i++)
		std::cout << ' ';
	std::cout << "Block" << std::endl;
	for(auto& subExpression : expression->Expressions) {
		if(subExpression.Type == ExpressionType::Block)
			PrintBlockExpression(reinterpret_cast<BlockExpression*>(subExpression.Data), indent + 2);
	}
}

void Parser::PrintProgramTree() {
	for(FunctionNode* function : m_ProgramNode.Functions) {
		std::cout << "  ";

		std::cout << "Function: " << function->Name << std::endl;
		PrintBlockExpression(&function->Block, 4);
	}
}

ProgramNode::~ProgramNode() {
	for(auto& fn : Functions)
		delete fn;
}

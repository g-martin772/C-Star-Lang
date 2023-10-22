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
			token = m_Lexer.GetNextToken();
			if(token.Type != TokenType::Identifier)
				return ResultType::InvalidSyntax;
			if(IsReserved(token.Content) || IsDataType(token.Content) || IsFunctionName(token.Content))
				return ResultType::InvalidSyntax;
			std::string name = token.Content;
			token = m_Lexer.GetNextToken();
			if(token.Type == TokenType::Semi) {
				// Variable Declaration
				DeclarationExpression* newExpression = new DeclarationExpression(type, name);
				m_CurrentBlock->Expressions.push_back({
					ExpressionType::Declaration,
					newExpression
				});
				continue;
			} else if(token.Type == TokenType::Equal) {
				// Variable Assignment
				DeclarationWithAssignmentExpression* newExpression = new DeclarationWithAssignmentExpression();
				newExpression->Identifier = name;
				newExpression->Type = type;

				// TODO: Get ValueExpression!

				m_CurrentBlock->Expressions.push_back({
					ExpressionType::DeclarationWithAssignment,
					newExpression
				});
				continue;
			} else {
				return ResultType::InvalidSyntax;
			}
		}

		// TODO: Keep track of variables xD and assignment

		// FunctionCalls
		if(token.Type == TokenType::Identifier && IsFunctionName(token.Content)) {
			std::string name = token.Content;
			token = m_Lexer.GetNextToken();
			if(token.Type != TokenType::ParenOpen)
				return ResultType::InvalidSyntax;
			FunctionCallExpression* newExpression = new FunctionCallExpression();
			newExpression->Name = name;

			// Parameters
			while (true) {
				token = m_Lexer.GetNextToken();

				// TODO: Get ValueExpression!

				token = m_Lexer.GetNextToken();
				if(token.Type == TokenType::Comma)
					continue;
				if(token.Type == TokenType::ParenClose)
					break;
				return ResultType::InvalidSyntax;
			}

			m_CurrentBlock->Expressions.push_back({
				ExpressionType::FunctionCall,
				newExpression
			});
			continue;
		}

		// TODO: If/Else/While

		// TODO: Return

		// TODO: Binary and Unary Operations
	} // End of Parse loop

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

void Indent(int indent) {
	for(int i = 0; i < indent; i++)
		std::cout << ' ';
}

void Parser::PrintExpression(Expression* expression, int indent) {
	if(expression == nullptr) {
		std::cout << std::endl;
		return;
	}
	switch (expression->Type) {
		case ExpressionType::Declaration: {
			Indent(indent + 2);
			DeclarationExpression* dec = reinterpret_cast<DeclarationExpression*>(expression->Data);
			std::cout << "type: " << dec->Type << " name: " << dec->Identifier << std::endl;
			break;
		}
		case ExpressionType::Assignment:
			break;
		case ExpressionType::DeclarationWithAssignment: {
			Indent(indent + 2);
			DeclarationWithAssignmentExpression* dec = reinterpret_cast<DeclarationWithAssignmentExpression*>(expression->Data);
			std::cout << "type: " << dec->Type << " name: " << dec->Identifier << std::endl;
			Indent(indent + 4);
			std::cout << "value: ";
			PrintExpression(dec->ValueExpression, 0);
			break;
		}
		case ExpressionType::Block:
			PrintBlockExpression(reinterpret_cast<BlockExpression*>(expression->Data), indent);
			break;
		case ExpressionType::FunctionCall:
			Indent(indent + 2);
			std::cout << "call: " << reinterpret_cast<FunctionCallExpression*>(expression->Data)->Name << std::endl;
			break;
		case ExpressionType::Value:
			break;
		case ExpressionType::UnaryOperation:
			break;
		case ExpressionType::BinaryOperation:
			break;
		case ExpressionType::Return:
			break;
		case ExpressionType::While:
			break;
		case ExpressionType::If:
			break;
		case ExpressionType::Else:
			break;
	}
}

void Parser::PrintBlockExpression(BlockExpression* expression, int indent) {
	Indent(indent);
	std::cout << "Block" << std::endl;
	for(auto& subExpression : expression->Expressions) {
		PrintExpression(&subExpression, indent + 2);
	}
}

void Parser::PrintProgramTree() {
	for(FunctionNode* function : m_ProgramNode.Functions) {
		Indent(2);

		std::cout << "Function: " << function->Name << std::endl;
		PrintBlockExpression(&function->Block, 4);
	}
}

bool Parser::IsFunctionName(const std::string &t) {
	for(auto& fn : m_ProgramNode.Functions)
		if(fn->Name == t)
			return true;
	return false;
}

ProgramNode::~ProgramNode() {
	for(auto& fn : Functions)
		delete fn;
}

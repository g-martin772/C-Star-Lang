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
		m_Token = m_Lexer.GetNextToken();

		if(m_Token.Type == TokenType::Invalid) {
			parsing = false;
			continue;
		}

		if(m_CurrentFunction == nullptr && m_Token.Type == TokenType::Identifier) {
			bool success = ParseFunctionHeader();
			if(!success)
				return ResultType::InvalidSyntax;
		}

		ParseFunction();
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
			std::cout << "type: " << dec->Type << ", name: " << dec->Identifier << std::endl;
			break;
		}
		case ExpressionType::Assignment:
			break;
		case ExpressionType::DeclarationWithAssignment: {
			Indent(indent + 2);
			InitializationExpression* dec = reinterpret_cast<InitializationExpression*>(expression->Data);
			std::cout << "type: " << dec->Type << ", name: " << dec->Identifier << std::endl;
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
		case ExpressionType::Return: {
			Indent(indent + 2);
			std::cout << "return: " << std::endl;

			ReturnExpression* returnExpression = reinterpret_cast<ReturnExpression*>(expression->Data);
			if(returnExpression->Value == nullptr) {
				Indent(indent + 4);
				std::cout << "void" << std::endl;
			} else {
				Expression ex = Expression{ExpressionType::Return, (void*)returnExpression->Value};
				PrintExpression(&ex, indent + 4);
			}
			break;
		}
		case ExpressionType::While: {
			Indent(indent + 2);
			std::cout << "while: " << std::endl;

			WhileExpression* whileExpression = reinterpret_cast<WhileExpression*>(expression->Data);
			PrintExpression(whileExpression->ConditionExpression, indent + 4);
			PrintExpression(whileExpression->BodyExpression, indent + 4);
			break;
		}
		case ExpressionType::If:{
			Indent(indent + 2);
			std::cout << "if: " << std::endl;

			IfExpression* ifExpression = reinterpret_cast<IfExpression*>(expression->Data);
			PrintExpression(ifExpression->ConditionExpression, indent + 4);
			PrintExpression(ifExpression->BodyExpression, indent + 4);
			break;
		}
		case ExpressionType::Else:{
			Indent(indent + 2);
			std::cout << "else: " << std::endl;

			ElseExpression* elseExpression = reinterpret_cast<ElseExpression*>(expression->Data);
			PrintExpression(elseExpression->BodyExpression, indent + 4);
			break;
		}
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

bool Parser::ParseFunction() {
	while (true) {
		// Block Open
		if(m_Token.Type == TokenType::CurlyOpen) {
			BlockExpression* newExpression = new BlockExpression(m_CurrentBlock);
			m_CurrentBlock->Expressions.push_back({ExpressionType::Block, newExpression});
			m_CurrentBlock = newExpression;
			continue;
		}

		// Block Close
		if(m_Token.Type == TokenType::CurlyClose) {
			if(m_CurrentBlock == &m_CurrentFunction->Block) {
				m_ProgramNode.Functions.push_back(m_CurrentFunction);
				m_CurrentFunction = nullptr;
				m_CurrentBlock = nullptr;
				return true;
			} else {
				m_CurrentBlock = m_CurrentBlock->Parent;
				continue;
			}
		}

		// Parse Declaration
		if(m_Token.Type == TokenType::Identifier && IsDataType(m_Token.Content)) {
			if(ParseDeclaration())
				continue;
		}

		// FunctionCalls
		if(m_Token.Type == TokenType::Identifier && IsFunctionName(m_Token.Content)) {
			if(ParseFunctionCall())
				continue;
		}

		// If
		if(m_Token.Type == TokenType::Identifier && m_Token.Content == "if") {
			if(ParseIfExpression())
				continue;
		}

		// While
		if(m_Token.Type == TokenType::Identifier && m_Token.Content == "while") {
			if(ParseWhileExpression())
				continue;
		}

		// Return
		if(m_Token.Type == TokenType::Identifier && m_Token.Content == "return") {
			if(ParseReturnExpression())
				continue;
		}

		// TODO: Binary and Unary Operations

		return false;
	}
}

bool Parser::ParseFunctionHeader() {
	m_CurrentFunction = new FunctionNode();

	// Type
	if (!IsDataType(m_Token.Content))
		return false;
	m_CurrentFunction->ReturnType = m_Token.Content;

	// Name
	m_Token = m_Lexer.GetNextToken();
	if (m_Token.Type != TokenType::Identifier)
		return false;
	if (IsReserved(m_Token.Content))
		return false;
	m_CurrentFunction->Name = m_Token.Content;

	// Parameters
	m_Token = m_Lexer.GetNextToken();
	if (m_Token.Type != TokenType::ParenOpen)
		return false;
	while (true) {
		m_Token = m_Lexer.GetNextToken();
		if (m_Token.Type != TokenType::Identifier || !IsDataType(m_Token.Content))
			return false;
		m_CurrentFunction->ParameterTypes.push_back(m_Token.Content);
		m_Token = m_Lexer.GetNextToken();
		if (m_Token.Type != TokenType::Identifier || IsDataType(m_Token.Content) || IsReserved(m_Token.Content))
			return false;
		m_CurrentFunction->Parameters.push_back(m_Token.Content);
		m_Token = m_Lexer.GetNextToken();
		if (m_Token.Type == TokenType::Comma)
			continue;
		if (m_Token.Type == TokenType::ParenClose)
			break;
		return false;
	}

	// Block Open
	m_Token = m_Lexer.GetNextToken();
	if(m_Token.Type != TokenType::CurlyOpen)
		return false;
	m_Token = m_Lexer.GetNextToken();
	m_CurrentBlock = &m_CurrentFunction->Block;

	return true;
}

bool Parser::ParseDeclaration() {
	std::string type = m_Token.Content;
	m_Token = m_Lexer.GetNextToken();
	if(m_Token.Type != TokenType::Identifier)
		return false;
	if(IsReserved(m_Token.Content) || IsDataType(m_Token.Content) || IsFunctionName(m_Token.Content))
		return false;
	std::string name = m_Token.Content;
	m_Token = m_Lexer.GetNextToken();

	if(m_Token.Type == TokenType::Semi) {
		// Variable Declaration
		DeclarationExpression* newExpression = new DeclarationExpression(type, name);
		m_CurrentBlock->Expressions.push_back({ExpressionType::Declaration,newExpression});
		return true;
	}

	if(m_Token.Type == TokenType::Equal) {
		// Variable Initialization
		InitializationExpression* newExpression = new InitializationExpression();
		newExpression->Identifier = name;
		newExpression->Type = type;

		// TODO: Get Value!

		m_CurrentBlock->Expressions.push_back({ExpressionType::DeclarationWithAssignment,newExpression});
		return true;
	}

	return false;
}

bool Parser::ParseFunctionCall() {
	std::string name = m_Token.Content;
	m_Token = m_Lexer.GetNextToken();
	if(m_Token.Type != TokenType::ParenOpen)
		return false;
	FunctionCallExpression* newExpression = new FunctionCallExpression();
	newExpression->Name = name;

	// Parameters
	while (true) {
		m_Token = m_Lexer.GetNextToken();

		// TODO: Get Value!

		m_Token = m_Lexer.GetNextToken();
		if(m_Token.Type == TokenType::Comma)
			continue;
		if(m_Token.Type == TokenType::ParenClose)
			break;
		return false;
	}

	m_Token = m_Lexer.GetNextToken();
	if(m_Token.Type != TokenType::Semi)
		return false;

	m_CurrentBlock->Expressions.push_back({ExpressionType::FunctionCall,newExpression});
	return true;
}

bool Parser::ParseIfExpression() {
	IfExpression* newExpression = new IfExpression();

	m_Token = m_Lexer.GetNextToken();
	if (m_Token.Type != TokenType::ParenOpen)
		return false;

	// TODO: Get Condition Expression
	while (m_Token.Type != TokenType::ParenClose)
		m_Token = m_Lexer.GetNextToken();

	// TODO: Get Body Expression

	m_CurrentBlock->Expressions.push_back({ExpressionType::If,newExpression});

	// Else
	if(m_Token.Type == TokenType::Identifier && m_Token.Content == "else") {
		ElseExpression* newExpression = new ElseExpression();

		// TODO: Get Body Expression

		m_CurrentBlock->Expressions.push_back({ExpressionType::If,newExpression});
	}

	return true;
}

bool Parser::ParseWhileExpression() {
	WhileExpression* newExpression = new WhileExpression();

	m_Token = m_Lexer.GetNextToken();
	if (m_Token.Type != TokenType::ParenOpen)
		return false;

	// TODO: Get Condition Expression
	while (m_Token.Type != TokenType::ParenClose)
		m_Token = m_Lexer.GetNextToken();

	m_Token = m_Lexer.GetNextToken();
	// TODO: Get Body Expression

	m_CurrentBlock->Expressions.push_back({ExpressionType::While,newExpression});
	return true;
}

bool Parser::ParseReturnExpression() {
	ReturnExpression* newExpression = new ReturnExpression();

	m_Token = m_Lexer.GetNextToken();
	if(m_Token.Type != TokenType::Semi) {
		newExpression->Value = GetValueExpression();
	}

	m_CurrentBlock->Expressions.push_back({ExpressionType::Return,newExpression});
	return true;
}

ValueExpression *Parser::GetValueExpression(const bool) {
	return nullptr;
}

ProgramNode::~ProgramNode() {
	for(auto& fn : Functions)
		delete fn;
}

BlockExpression::~BlockExpression() {
	for(auto& exp : Expressions){
		switch (exp.Type) {
			case ExpressionType::Declaration:
				delete reinterpret_cast<DeclarationExpression*>(exp.Data);
				break;
			case ExpressionType::Assignment:
				delete reinterpret_cast<AssignmentExpression*>(exp.Data);
				break;
			case ExpressionType::DeclarationWithAssignment:
				delete reinterpret_cast<InitializationExpression*>(exp.Data);
				break;
			case ExpressionType::Block:
				delete reinterpret_cast<BlockExpression*>(exp.Data);
				break;
			case ExpressionType::FunctionCall:
				delete reinterpret_cast<FunctionCallExpression*>(exp.Data);
				break;
			case ExpressionType::Value:
				delete reinterpret_cast<ValueExpression*>(exp.Data);
				break;
			case ExpressionType::UnaryOperation:
				delete reinterpret_cast<UnaryOperationExpression*>(exp.Data);
				break;
			case ExpressionType::BinaryOperation:
				delete reinterpret_cast<BinaryOperationExpression*>(exp.Data);
				break;
			case ExpressionType::Return:
				delete reinterpret_cast<ReturnExpression*>(exp.Data);
				break;
			case ExpressionType::While:
				delete reinterpret_cast<WhileExpression*>(exp.Data);
				break;
			case ExpressionType::If:
				delete reinterpret_cast<IfExpression*>(exp.Data);
				break;
			case ExpressionType::Else:
				delete reinterpret_cast<ElseExpression*>(exp.Data);
				break;
		}
	}
}

ValueExpression::~ValueExpression() {
	if(FunctionCall != nullptr)
		delete FunctionCall;
}

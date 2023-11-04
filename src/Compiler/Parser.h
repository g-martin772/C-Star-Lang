#pragma once

#include <vector>
#include "Lexer.h"
#include "ErrorHandling/CompilerResult.h"

struct Expression;

enum class ExpressionType {
    Declaration,
    Assignment,
    DeclarationWithAssignment,
    Block,
    FunctionCall,
    Value,
    UnaryOperation,
    BinaryOperation,
    Return,
    While,
    If,
    Else,
};

struct DeclarationExpression {
	explicit DeclarationExpression(std::string type, std::string identifier)
		: Identifier(identifier), Type(type) {}

    std::string Identifier;
    std::string Type;
};

struct AssignmentExpression {
    std::string Identifier;
    Expression* ValueExpression;
};

struct InitializationExpression {
	std::string Type;
    std::string Identifier;
    Expression* ValueExpression;
};

struct BlockExpression {
	explicit BlockExpression(BlockExpression* parent = nullptr)
		: Parent(parent) {}

	~BlockExpression();

    std::vector<Expression> Expressions;
	BlockExpression* Parent = nullptr;
};

struct FunctionCallExpression {
    std::string Name;
    std::vector<Expression> Arguments;
};

struct ValueExpression {
	~ValueExpression();
	std::string StringLiteral;
	long ValueLiteral;
	FunctionCallExpression* FunctionCall;
};

struct UnaryOperationExpression {
	// TODO
    ExpressionType operation;
    Expression* operand;
};

struct BinaryOperationExpression {
	// TODO
    ExpressionType operation;
    Expression* leftOperand;
    Expression* rightOperand;
};

struct ReturnExpression {
    ValueExpression* Value;
};

struct WhileExpression {
    Expression* ConditionExpression;
    Expression* BodyExpression;
};

struct IfExpression {
    Expression* ConditionExpression;
    Expression* BodyExpression;
};

struct ElseExpression {
    Expression* BodyExpression;
};

struct Expression {
    ExpressionType Type;
    void* Data;
};

struct FunctionNode {
    std::string Name;
    std::string ReturnType;
    std::vector<std::string> ParameterTypes;
    std::vector<std::string> Parameters;
	BlockExpression Block;
};

struct ProgramNode {
	~ProgramNode();
    std::vector<FunctionNode*> Functions;
};

class Parser {
public:
	explicit Parser(Lexer& lexer);
    CompilerResult Parse();
	void PrintProgramTree();
private:
	bool ParseFunction();
	bool ParseFunctionHeader();
	bool ParseDeclaration();
	bool ParseFunctionCall();
	bool ParseIfExpression();
	bool ParseWhileExpression();
	bool ParseReturnExpression();
	ValueExpression* GetValueExpression(bool const = false);


	static bool IsReserved(const std::string& t);
	static bool IsDataType(const std::string& t);
	bool IsFunctionName(const std::string& t);

	void PrintBlockExpression(BlockExpression* expression, int indent);
	void PrintExpression(Expression* expression, int indent);

	Lexer& m_Lexer;
	Token m_Token;
    ProgramNode m_ProgramNode;
	FunctionNode* m_CurrentFunction = nullptr;
	BlockExpression* m_CurrentBlock = nullptr;
};

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
    std::string Identifier;
    std::string Type;
};

struct AssignmentExpression {
    std::string Identifier;
    Expression* ValueExpression;
};

struct DeclarationWithAssignmentExpression {
    std::string Identifier;
    Expression* ValueExpression;
};

struct BlockExpression {
    std::vector<Expression> Statements;
};

struct FunctionCallExpression {
    std::string FunctionName;
    std::vector<Expression> Arguments;
};

struct ValueExpression {
    union {
        std::string StringLiteral;
        long ValueLiteral;
        Expression* FunctionCall;
    };
};

struct UnaryOperationExpression {
    ExpressionType operation;
    Expression* operand;
};

struct BinaryOperationExpression {
    ExpressionType operation;
    Expression* leftOperand;
    Expression* rightOperand;
};

struct ReturnExpression {
    Expression* ValueExpression;
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
    std::string Identifier;
    std::string ReturnType;
    std::vector<std::string> ParameterTypes;
    std::vector<std::string> Parameters;
    std::vector<Expression> Expressions;
};

struct ProgramNode {
    std::vector<FunctionNode> Functions;
};

class Parser {
public:
    CompilerResult ConsumeToken(const Token& token);
private:
    ProgramNode m_ProgramNode;
};

#pragma once

#include <vector>
#include <map>
#include <tuple>
#include <memory>

#include "token.h"
#include "tokentype.h"

class Node {
public:
    Token token;

    Node() : token(Token()) {}
    Node(Token token) : token(token) {}

    virtual std::string TokenLiteral() const;
};

class Statement : public Node {
public:
    Statement(Token token) : Node(token) {}
};
class Expression : public Node {
public:
    Expression(Token token) : Node(token) {}
};

class Program : public Node {
public:
    std::vector<std::shared_ptr<Statement>> statements;

    std::string TokenLiteral() const override;
};

class TypeAnnotation : public Node {
public:
    bool is_maybe;
};

class BlockStatement : public Statement {
public:
    std::vector<std::shared_ptr<Statement>> statements;
};

class PrefixExpression : public Expression {
public:
    std::string op;
    std::shared_ptr<Expression> right;
};

class InfixExpression : public Expression {
public:
    std::shared_ptr<Expression> left;
    std::string op;
    std::shared_ptr<Expression> right;
};

class Nil : public Expression {
};

class Identifier : public Expression {
public:
    std::string value;
};

class IntegerLiteral : public Expression {
public:
    int value;
};

class FloatLiteral : public Expression {
public:
    float value;
};

class Boolean : public Expression {
public:
    bool value;
};

class StringLiteral : public Expression {
public:
    std::string literal;
};

class MapLiteral : public Expression {
public:
    std::map<std::shared_ptr<Expression>, std::shared_ptr<Expression>> pairs;
};

class ListLiteral : public Expression {
public:
    std::vector<std::shared_ptr<Expression>> elements;
};

class IndexExpression : public Expression {
public:
    std::shared_ptr<Expression> left;
    std::vector<std::shared_ptr<Expression>> index;
};

class CallExpression : public Expression {
public:
    std::shared_ptr<Identifier> function;
    std::vector<std::shared_ptr<Expression>> args;
};

class IfExpression : public Expression {
public:
    std::shared_ptr<Expression> condition;
    std::shared_ptr<BlockStatement> consequence;
    std::shared_ptr<BlockStatement> alternative;
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Token token, std::shared_ptr<Expression> expr)
        : Statement(token), expression(expr) {} 
    std::shared_ptr<Expression> expression;
};

class WhileStatement : public Statement {
public:
    std::shared_ptr<Expression> condition;
    std::shared_ptr<BlockStatement> body;
};

class FunctionStatement : public Statement {
public:
    std::shared_ptr<Identifier> name;
    std::vector<std::tuple<Identifier, std::shared_ptr<TypeAnnotation>>> params;
    std::shared_ptr<BlockStatement> body;
    std::shared_ptr<TypeAnnotation> return_type;
};

class VarStatement : public Statement {
public:
    std::shared_ptr<Identifier> name;
    std::shared_ptr<Expression> value;
};

class IndexAssignment : public Statement {
public:
    std::shared_ptr<Expression> left;
    std::vector<std::shared_ptr<Expression>> index;
};

class ReturnStatement : public Statement {
public:
    std::shared_ptr<Expression> return_value;
};

class PassStatement : public Statement {};

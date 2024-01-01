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

    ~Node() = default;

    virtual void print(std::ostream& os) const = 0;
};
std::ostream& operator<<(std::ostream& os, const Node& node);

class Statement : public Node {
public:
    Statement(Token token) : Node(token) {}
    virtual void print(std::ostream& os) const override { os << token; };
};
class Expression : public Node {
public:
    Expression(Token token) : Node(token) {}
    virtual void print(std::ostream& os) const override { os << token; };
};

class Program : public Node {
public:
    std::vector<std::shared_ptr<Statement>> statements;
    void print(std::ostream& os) const override { for(const auto& stmt : statements) os << *stmt << "\n"; }
};

class TypeAnnotation : public Node {
public:
    bool is_maybe;
    void print(std::ostream& os) const override { os << token.literal << (is_maybe ? "?" : ""); }
};

class BlockExpression : public Expression {
public:
    BlockExpression(Token token, std::vector<std::shared_ptr<Statement>> statements)
        : Expression(token), statements(statements) {}
    std::vector<std::shared_ptr<Statement>> statements;
    void print(std::ostream& os) const override { for(const auto& stmt : statements) os << *stmt; };
};

class PrefixExpression : public Expression {
public:
    PrefixExpression(Token token, const std::string& op, std::shared_ptr<Expression> right)
        : Expression(token), op(op), right(right) {}
    std::string op;
    std::shared_ptr<Expression> right;
    void print(std::ostream& os) const override { os << op << " " << *right; }
};

class InfixExpression : public Expression {
public:
    InfixExpression(Token token, std::shared_ptr<Expression> left, const std::string& op, std::shared_ptr<Expression> right)
        : Expression(token), left(left), op(op), right(right) {}
    std::shared_ptr<Expression> left;
    std::string op;
    std::shared_ptr<Expression> right;
    void print(std::ostream& os) const override { os << *left << " " << op << " " << *right; }
};

class Nil : public Expression {
public:
    Nil(Token token) : Expression(token) {}
    void print(std::ostream& os) const override { os << "nil"; };
};

class Identifier : public Expression {
public:
    Identifier(Token token, const std::string& value)
        : Expression(token), value(value) {}
    std::string value;
    void print(std::ostream& os) const override { os << value; };
};

class IntegerLiteral : public Expression {
public:
    IntegerLiteral(Token token, int value)
        : Expression(token), value(value) {}
    int value;
    void print(std::ostream& os) const override { os << value; }
};

class FloatLiteral : public Expression {
public:
    FloatLiteral(Token token, float value)
        : Expression(token), value(value) {}
    float value;
    void print(std::ostream& os) const override { os << value; };
};

class Boolean : public Expression {
public:
    Boolean(Token token, bool value)
        : Expression(token), value(value) {}
    bool value;
    void print(std::ostream& os) const override { os << (value ? "true" : "false"); }
};

class StringLiteral : public Expression {
public:
    StringLiteral(Token token, const std::string& literal)
        : Expression(token), literal(literal) {}
    std::string literal;
    void print(std::ostream& os) const override { os << "\"" << literal << "\""; };
};

class MapLiteral : public Expression {
public:
    MapLiteral(Token token, std::map<std::shared_ptr<Expression>, std::shared_ptr<Expression>> pairs)
        : Expression(token), pairs(pairs) {}
    std::map<std::shared_ptr<Expression>, std::shared_ptr<Expression>> pairs;
    void print(std::ostream& os) const override {
        os << "{";
        auto pair = pairs.begin();
        auto end = pairs.end();
        for(; pair != end; pair++) {
            os << *(pair->first) << ": " << *(pair->second);
            if(std::next(pair) != end)
                os << ", ";
        }
        os << "}";
    };
};

class ListLiteral : public Expression {
public:
    ListLiteral(Token token, std::vector<std::shared_ptr<Expression>> elements)
        : Expression(token), elements(elements) {}
    std::vector<std::shared_ptr<Expression>> elements;
    void print(std::ostream& os) const override {
        os << "[";
        for(auto i = 0; i<elements.size(); i++) {
            os << *elements[i];
            if(i + 1 < elements.size()) os << ",";
        }
        os << "]";
    };
};

class IndexExpression : public Expression {
public:
    IndexExpression(Token token, std::shared_ptr<Expression> left, std::vector<std::shared_ptr<Expression>> indices)
        : Expression(token), left(left), indices(indices) {}
    std::shared_ptr<Expression> left;
    std::vector<std::shared_ptr<Expression>> indices;
    void print(std::ostream& os) const override {
        os << *left;
        for(const auto& i : indices) {
            os << "[" << *i << "]";
        }
    }
};

class CallExpression : public Expression {
public:
    CallExpression(Token token, std::shared_ptr<Identifier> function, std::vector<std::shared_ptr<Expression>> args)
        : Expression(token), function(function), args(args) {}
    std::shared_ptr<Identifier> function;
    std::vector<std::shared_ptr<Expression>> args;
    void print(std::ostream& os) const override {
        os << *function;
        os << "(";
        for(auto i = 0; i<args.size(); i++) {
            os << *args[i];
            if(i + 1 < args.size()) os << ",";
        }
        os << ")";
    };
};

class IfExpression : public Expression {
public:
    IfExpression(Token token, std::shared_ptr<Expression> condition, std::shared_ptr<BlockExpression> consequence, std::shared_ptr<BlockExpression> alternative)
        : Expression(token), consequence(consequence), alternative(alternative) {}
    std::shared_ptr<Expression> condition;
    std::shared_ptr<BlockExpression> consequence;
    std::shared_ptr<BlockExpression> alternative;
    void print(std::ostream& os) const override {};
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Token token, std::shared_ptr<Expression> expr)
        : Statement(token), expression(expr) {} 
    std::shared_ptr<Expression> expression;
    void print(std::ostream& os) const override { os << *expression; }
};

class WhileStatement : public Statement {
public:
    std::shared_ptr<Expression> condition;
    std::shared_ptr<BlockExpression> body;
    void print(std::ostream& os) const override {};
};

class FunctionStatement : public Statement {
public:
    std::shared_ptr<Identifier> name;
    std::vector<std::tuple<Identifier, std::shared_ptr<TypeAnnotation>>> params;
    std::shared_ptr<BlockExpression> body;
    std::shared_ptr<TypeAnnotation> return_type;
    void print(std::ostream& os) const override {};
};

class VarStatement : public Statement {
public:
    std::shared_ptr<Identifier> name;
    std::shared_ptr<TypeAnnotation> type;
    std::shared_ptr<Expression> value;
    void print(std::ostream& os) const override { os << "var " << *name << ": " << *type << " = " << *value; }
};

class AssignmentStatement : public Statement {
public:
    AssignmentStatement(Token token, std::shared_ptr<Identifier> name, std::shared_ptr<Expression> value)
        : Statement(token), name(name), value(value) {}
    std::shared_ptr<Identifier> name;
    std::shared_ptr<Expression> value;
    void print(std::ostream& os) const override {os << *name << " = " << *value ; }
};

class IndexAssignment : public Statement {
public:
    std::shared_ptr<Identifier> name;
    std::vector<std::shared_ptr<Expression>> indices;
    std::shared_ptr<Expression> value;
    void print(std::ostream& os) const override {
        os << name;
        for(const auto& i : indices) os << "[" << *i << "]";
        os << " = " << *value;
    }
};

class ReturnStatement : public Statement {
public:
    std::shared_ptr<Expression> return_value;
    void print(std::ostream& os) const override { os << "return " << *return_value; }
};

class PassStatement : public Statement {
public:
    PassStatement(Token token) : Statement(token) {}
    void print(std::ostream& os) const override { os << "pass"; }
};

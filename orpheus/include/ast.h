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
    void print(std::ostream& os) const override { 
        for(int i=0; i<statements.size(); i++) {
            os << *statements[i];
            if(i+1<statements.size()) os << "\n";
        }
    }
};

class TypeLiteral : public Expression {
public:
    TypeLiteral(Token token, bool is_maybe) : Expression(token), is_maybe(is_maybe) {}
    bool is_maybe;
    void print(std::ostream& os) const override { os << token.literal << (is_maybe ? "?" : ""); }
};

class BlockExpression : public Expression {
public:
    BlockExpression(Token token, std::vector<std::shared_ptr<Statement>> statements)
        : Expression(token), statements(statements) {}
    std::vector<std::shared_ptr<Statement>> statements;
    void print(std::ostream& os) const override {
        if(statements.size() == 1) {
            os << "-> " << *statements[0];
        } else {
            os << "->\n";
            for(int i=0; i<statements.size(); i++) {
                os << *statements[i] << "\n";
                //if(i+1<statements.size()) os << "\n";
            }
            os << "<-";
        }
    }
};

class PrefixExpression : public Expression {
public:
    PrefixExpression(Token token, const std::string& op, std::shared_ptr<Node> right)
        : Expression(token), op(op), right(right) {}
    std::string op;
    std::shared_ptr<Node> right;
    void print(std::ostream& os) const override { os << op << " " << *right; }
};

class InfixExpression : public Expression {
public:
    InfixExpression(Token token, std::shared_ptr<Node> left, const std::string& op, std::shared_ptr<Node> right)
        : Expression(token), left(left), op(op), right(right) {}
    std::shared_ptr<Node> left;
    std::string op;
    std::shared_ptr<Node> right;
    void print(std::ostream& os) const override { 
        os << "(" << *left << " " << op << " " << *right << ")"; 
    }
};

class NilLiteral : public Expression {
public:
    NilLiteral(Token token) : Expression(token) {}
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
    MapLiteral(Token token, std::map<std::shared_ptr<Node>, std::shared_ptr<Node>> pairs)
        : Expression(token), pairs(pairs) {}
    std::map<std::shared_ptr<Node>, std::shared_ptr<Node>> pairs;
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
    ListLiteral(Token token, std::vector<std::shared_ptr<Node>> elements)
        : Expression(token), elements(elements) {}
    std::vector<std::shared_ptr<Node>> elements;
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
    IndexExpression(Token token, std::shared_ptr<Node> left, std::vector<std::shared_ptr<Node>> indices)
        : Expression(token), left(left), indices(indices) {}
    std::shared_ptr<Node> left;
    std::vector<std::shared_ptr<Node>> indices;
    void print(std::ostream& os) const override {
        os << *left;
        for(const auto& i : indices) {
            os << "[" << *i << "]";
        }
    }
};

class CallExpression : public Expression {
public:
    CallExpression(Token token, std::shared_ptr<Identifier> left, std::vector<std::shared_ptr<Node>> args)
        : Expression(token), left(left), args(args) {}
    std::shared_ptr<Identifier> left;
    std::vector<std::shared_ptr<Node>> args;
    void print(std::ostream& os) const override {
        os << *left;
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
    IfExpression(Token token, std::shared_ptr<Node> condition, std::shared_ptr<Node> consequence, std::shared_ptr<Node> alternative)
        : Expression(token), condition(condition), consequence(consequence), alternative(alternative) {}
    std::shared_ptr<Node> condition;
    std::shared_ptr<Node> consequence;
    std::shared_ptr<Node> alternative;
    void print(std::ostream& os) const override {
        os << "if " << *condition << " ";
        os << *consequence;
        if(alternative != nullptr) {
            os << "else " << *alternative;
        }
    };
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Token token, std::shared_ptr<Node> expr)
        : Statement(token), expression(expr) {} 
    std::shared_ptr<Node> expression;
    void print(std::ostream& os) const override { os << *expression; }
};

class WhileStatement : public Statement {
public:
    WhileStatement(Token token, std::shared_ptr<Node> condition, std::shared_ptr<Node> body)
        : Statement(token), condition(condition), body(body) {}
    std::shared_ptr<Node> condition;
    std::shared_ptr<Node> body;
    void print(std::ostream& os) const override {
        os << "while " << *condition << " ";
        os << *body;
    };
};

class FunctionStatement : public Statement {
public:
    FunctionStatement(Token token, std::shared_ptr<Identifier> name, std::vector<std::pair<std::shared_ptr<Identifier>, std::shared_ptr<TypeLiteral>>> params, std::shared_ptr<Node> body, std::shared_ptr<Node> return_type)
        : Statement(token), name(name), params(params), body(body), return_type(return_type) {}
    std::shared_ptr<Identifier> name;
    std::vector<std::pair<std::shared_ptr<Identifier>, std::shared_ptr<TypeLiteral>>> params;
    std::shared_ptr<Node> body;
    std::shared_ptr<Node> return_type;
    void print(std::ostream& os) const override {
        os << "func " << *name << "(";
        for(const auto& p : params) {
            os << *(std::get<0>(p)) << ": " << *(std::get<1>(p)) << ", ";
        }
        os << ") : " << *return_type << "\n";
        os << *body;
    };
};

class VarStatement : public Statement {
public:
    VarStatement(Token token, std::shared_ptr<Identifier> name, std::shared_ptr<TypeLiteral> type, std::shared_ptr<Node> value)
        : Statement(token), name(name), type(type), value(value) {}
    std::shared_ptr<Identifier> name;
    std::shared_ptr<TypeLiteral> type;
    std::shared_ptr<Node> value;
    void print(std::ostream& os) const override { os << "var " << *name << ": " << *type << " = " << *value; }
};

class AssignmentStatement : public Statement {
public:
    AssignmentStatement(Token token, std::shared_ptr<Identifier> name, std::shared_ptr<Node> value)
        : Statement(token), name(name), value(value) {}
    std::shared_ptr<Identifier> name;
    std::shared_ptr<Node> value;
    void print(std::ostream& os) const override {os << *name << " = " << *value ; }
};

class IndexAssignment : public Statement {
public:
    IndexAssignment(Token token, std::shared_ptr<Identifier> name, std::vector<std::shared_ptr<Node>> indices, std::shared_ptr<Node> value)
        : Statement(token), name(name), indices(indices), value(value) {}
    std::shared_ptr<Identifier> name;
    std::vector<std::shared_ptr<Node>> indices;
    std::shared_ptr<Node> value;
    void print(std::ostream& os) const override {
        os << *name;
        for(const auto& i : indices) os << "[" << *i << "]";
        os << " = " << *value;
    }
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(Token token, std::shared_ptr<Node> return_value)
        : Statement(token), return_value(return_value) {}
    std::shared_ptr<Node> return_value;
    void print(std::ostream& os) const override { os << "return " << *return_value; }
};

class PassStatement : public Statement {
public:
    PassStatement(Token token) : Statement(token) {}
    void print(std::ostream& os) const override { os << "pass"; }
};

#pragma once

#include <unordered_map>
#include <vector>
#include <sstream>
#include <memory>
#include <functional>

#include "tokentype.h"
#include "token.h"
#include "error_handler.h"
#include "ast.h"

enum class PrecLevel {
    LOWEST,
    AND_OR,
    EQUALS,
    IS,
    LESS_GREATER,
    SUM,
    PRODUCT, 
    PREFIX,
    CALL,
    INDEX
};

class Parser {
    using PrefixFn = std::shared_ptr<Node> (Parser::*)();
    using InfixFn = std::shared_ptr<Node> (Parser::*)(std::shared_ptr<Node> left);

    std::vector<Token> tokens;
    Token current_token;
    Token peek_token;
    int t_idx;

    std::unordered_map<TokenType, PrefixFn> prefix_fns;
    std::unordered_map<TokenType, InfixFn> infix_fns;


public:
    Parser(const std::vector<Token>& tokens = {});

    void RegisterPrefix(TokenType type, PrefixFn fn);
    void RegisterInfix(TokenType type, InfixFn fn);
    void RegisterAll();

    Program ParseProgram();

    std::shared_ptr<Statement> ParseStatement();
    
    std::shared_ptr<VarStatement>        ParseVarStatement();
    std::shared_ptr<ReturnStatement>     ParseReturnStatement();
    std::shared_ptr<FunctionStatement>   ParseFunctionStatement();
    std::shared_ptr<ExpressionStatement> ParseExpressionStatement();
    std::shared_ptr<PassStatement>       ParsePassStatement();
    std::shared_ptr<WhileStatement>      ParseWhileStatement();

    std::shared_ptr<Node> ParseExpression(PrecLevel precedence);
    std::shared_ptr<Node> ParsePrefixExpression();
    std::shared_ptr<Node> ParseInfixExpression(std::shared_ptr<Node> left);
    
    std::shared_ptr<Node> ParseBlockExpression();
    std::shared_ptr<Node> ParseArrowBlock();
    std::shared_ptr<Node> ParseGroupedExpression();
    std::shared_ptr<Node> ParseIfExpression();

    std::shared_ptr<Node> ParseIdentifier();
    std::shared_ptr<Node> ParseInteger();
    std::shared_ptr<Node> ParseFloat();
    std::shared_ptr<Node> ParseString();
    std::shared_ptr<Node> ParseBoolean();
    std::shared_ptr<Node> ParseNil();
    std::shared_ptr<Node> ParseTypeLiteral();

    std::vector<std::shared_ptr<Node>> ParseExpressionList(TokenType end);

    std::shared_ptr<Node> ParseListLiteral();
    std::shared_ptr<Node> ParseMapLiteral();

    std::shared_ptr<Node> ParseCallExpression(std::shared_ptr<Node> left);
    std::shared_ptr<Node> ParseIndexExpression(std::shared_ptr<Node> left);
    
    void Advance();
    bool CurrentTokenIs(TokenType type) const;
    bool PeekTokenIs(TokenType type) const;
    bool DoublePeekIs(TokenType type) const;
    bool ExpectPeek(TokenType type);
    bool PeekIsWhitespace() const;
    bool CurrentIsWhitespace() const;
    bool IsCurrentEnd() const;
    bool IsPeekEnd() const;
    PrecLevel PeekPrecedence() const;
    PrecLevel CurrentPrecedence() const;
    void IgnoreWhitespace();
};

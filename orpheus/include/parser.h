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
    using PrefixFn = std::shared_ptr<Expression> (Parser::*)();
    using InfixFn = std::shared_ptr<Expression> (Parser::*)(std::shared_ptr<Expression> left);

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
    std::shared_ptr<AssignmentStatement> ParseAssignmentStatement();

    std::shared_ptr<Expression> ParseExpression(PrecLevel precedence);
    std::shared_ptr<Expression> ParsePrefixExpression();
    std::shared_ptr<Expression> ParseInfixExpression(std::shared_ptr<Expression> left);
    
    std::shared_ptr<Expression> ParseBlockExpression();
    std::shared_ptr<Expression> ParseArrowBlock();
    std::shared_ptr<Expression> ParseGroupedExpression();
    std::shared_ptr<Expression> ParseIfExpression();

    std::shared_ptr<Expression> ParseIdentifier();
    std::shared_ptr<Expression> ParseInteger();
    std::shared_ptr<Expression> ParseFloat();
    std::shared_ptr<Expression> ParseString();
    std::shared_ptr<Expression> ParseBoolean();
    std::shared_ptr<Expression> ParseNil();
    std::shared_ptr<Expression> ParseTypeLiteral();

    std::vector<std::shared_ptr<Expression>> ParseExpressionList(TokenType end);

    std::shared_ptr<Expression> ParseListLiteral();
    std::shared_ptr<Expression> ParseMapLiteral();

    std::shared_ptr<Expression> ParseCallExpression(std::shared_ptr<Expression> left);
    std::shared_ptr<Expression> ParseIndexExpression(std::shared_ptr<Expression> left);
    
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

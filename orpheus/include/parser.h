#pragma once

#include <unordered_map>
#include <vector>
#include <sstream>

#include "tokentype.h"
#include "token.h"
#include "error_handler.h"
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
    using PrefixFn = void (Parser::*)();
    using InfixFn = void (Parser::*)();

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

    void Advance();
    bool CurrentTokenIs(TokenType type);
    bool PeekTokenIs(TokenType type);
    bool DoublePeekIs(TokenType type);
    bool ExpectPeek(TokenType type);
    bool PeekIsWhitespace();
    bool CurrentIsWhitespace();
    bool IsCurrentEnd();
    bool IsPeekEnd();
    PrecLevel PeekPrecedence();
    PrecLevel CurrentPrecedence();
    void IgnoreWhitespace();
};

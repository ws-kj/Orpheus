#pragma once

#include "tokentype.h"

class Token {
public:
    TokenType type;
    std::string literal;
    int line;

    Token(TokenType type, const std::string& literal, int line) 
        : type(type), literal(literal), line(line) {}

    friend std::ostream& operator<<(std::ostream& out, const Token& token);

};


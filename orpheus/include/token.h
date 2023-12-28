#pragma once

#include "tokentype.h"

class Token {
    TokenType type;
    std::string literal;
    int line;

    std::string to_string() const {
        return NULL;
    }
};


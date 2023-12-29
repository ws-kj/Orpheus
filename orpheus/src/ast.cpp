#include "../include/ast.h"

std::string Node::TokenLiteral() const {
    return token.literal;
}

std::string Program::TokenLiteral() const {
    if(!statements.empty()) {
        return statements[0]->TokenLiteral();
    }
    return "";
}



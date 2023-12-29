#include "../include/parser.h"

std::unordered_map<TokenType, PrecLevel> precedences = {
    {TokenType::AND, PrecLevel::AND_OR},
    {TokenType::OR, PrecLevel::AND_OR},
    {TokenType::EQUAL_EQUAL, PrecLevel::EQUALS},
    {TokenType::BANG_EQUAL, PrecLevel::EQUALS},
    {TokenType::IS, PrecLevel::IS},
    {TokenType::LESS, PrecLevel::LESS_GREATER},
    {TokenType::LESS_EQUAL, PrecLevel::LESS_GREATER},
    {TokenType::GREATER, PrecLevel::LESS_GREATER},
    {TokenType::GREATER_EQUAL, PrecLevel::LESS_GREATER},
    {TokenType::PLUS, PrecLevel::SUM},
    {TokenType::MINUS, PrecLevel::SUM},
    {TokenType::SLASH, PrecLevel::PRODUCT},
    {TokenType::STAR, PrecLevel::PRODUCT},
    {TokenType::PERCENT, PrecLevel::PRODUCT},
    {TokenType::LPAREN, PrecLevel::CALL},
    {TokenType::LBRACKET, PrecLevel::INDEX}
};

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), t_idx(0) {
    RegisterAll();

    if(!tokens.empty()) {
        current_token = tokens[0];
        if(tokens.size() > 1) 
            peek_token = tokens[1];
    }
}

void Parser::RegisterPrefix(TokenType type, PrefixFn fn) {
    prefix_fns[type] = fn;
}

void Parser::RegisterInfix(TokenType type, InfixFn fn) {
    infix_fns[type] = fn;
}

void Parser::RegisterAll() {
    
}

void Parser::Advance() {
    t_idx++;
    current_token = tokens[t_idx];
    if(t_idx >= tokens.size()-1)
        peek_token = Token();
    else
        peek_token = tokens[t_idx+1];
}

bool Parser::CurrentTokenIs(TokenType type) {
    return current_token.type == type;
}
bool Parser::PeekTokenIs(TokenType type) {
    return peek_token.type == type;
}

bool Parser::DoublePeekIs(TokenType type) {
    int n = t_idx+2;
    if(n < tokens.size()) {
        return tokens[n].type == type;
    }
    return false;
}

bool Parser::ExpectPeek(TokenType type) {
    if(PeekTokenIs(type)) {
        Advance();
        return true;
    }
    std::stringstream msg; 
    msg << "Error: expected " << type << ", got " << peek_token.type;
    ErrorHandler::Error(peek_token.line, msg.str());
    return false;
}

bool Parser::PeekIsWhitespace() {
    return PeekTokenIs(TokenType::NEWLINE) || PeekTokenIs(TokenType::INDENT) 
        || PeekTokenIs(TokenType::DEDENT);
}

bool Parser::CurrentIsWhitespace() {
    return CurrentTokenIs(TokenType::NEWLINE) 
        || CurrentTokenIs(TokenType::INDENT) || CurrentTokenIs(TokenType::DEDENT);
}

bool Parser::IsCurrentEnd() {
    return CurrentTokenIs(TokenType::NEWLINE) || CurrentTokenIs(TokenType::ENDFILE);
}

bool Parser::IsPeekEnd() {
    return PeekTokenIs(TokenType::NEWLINE) || PeekTokenIs(TokenType::ENDFILE);
}

PrecLevel Parser::PeekPrecedence() {
    if(precedences.find(peek_token.type) != precedences.end()) {
        return precedences[peek_token.type];
    }
    return PrecLevel::LOWEST;
}

PrecLevel Parser::CurrentPrecedence() {
    if(precedences.find(current_token.type) != precedences.end()) {
        return precedences[current_token.type];
    }
    return PrecLevel::LOWEST;
}

void Parser::IgnoreWhitespace() {
    while (PeekIsWhitespace()) {
        Advance();
    }
}

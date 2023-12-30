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
    RegisterPrefix(TokenType::IDENTIFIER, &Parser::ParseIdentifier);
    RegisterPrefix(TokenType::INTEGER, &Parser::ParseInteger);
    RegisterPrefix(TokenType::FLOAT, &Parser::ParseFloat);
    RegisterPrefix(TokenType::STRING, &Parser::ParseString);
    RegisterPrefix(TokenType::TRUE, &Parser::ParseBoolean);
    RegisterPrefix(TokenType::FALSE, &Parser::ParseBoolean);
    RegisterPrefix(TokenType::NIL, &Parser::ParseNil);
    RegisterPrefix(TokenType::LBRACKET, &Parser::ParseListLiteral);
    RegisterPrefix(TokenType::LBRACE, &Parser::ParseMapLiteral); 

    RegisterPrefix(TokenType::T_INT, &Parser::ParseTypeAnnotation);
    RegisterPrefix(TokenType::T_FLOAT, &Parser::ParseTypeAnnotation);
    RegisterPrefix(TokenType::T_STR, &Parser::ParseTypeAnnotation);
    RegisterPrefix(TokenType::T_MAP, &Parser::ParseTypeAnnotation);
    RegisterPrefix(TokenType::T_LIST, &Parser::ParseTypeAnnotation);
    RegisterPrefix(TokenType::T_BOOL, &Parser::ParseTypeAnnotation);
    RegisterPrefix(TokenType::FUNC, &Parser::ParseTypeAnnotation);

    RegisterPrefix(TokenType::ARROW, &Parser::ParseBlockExpression);
    RegisterPrefix(TokenType::IF, &Parser::ParseIfExpression);
 
    RegisterInfix(TokenType::LBRACKET, &Parser::ParseIndexExpression);
    RegisterInfix(TokenType::LPAREN, &Parser::ParseCallExpression);

    RegisterInfix(TokenType::EQUAL_EQUAL, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::BANG_EQUAL, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::LESS, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::LESS_EQUAL, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::GREATER, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::GREATER_EQUAL, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::PLUS, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::MINUS, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::SLASH, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::STAR, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::PERCENT, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::AND, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::OR, &Parser::ParseInfixExpression);
    RegisterInfix(TokenType::IS, &Parser::ParseInfixExpression);
}

Program Parser::ParseProgram() {
    Program program;
    if(tokens.empty()) return program;

    while(current_token.type != TokenType::ENDFILE) {
        std::shared_ptr<Statement> statement = ParseStatement();
        if(statement != nullptr)
            program.statements.push_back(statement);
        if(current_token.type == TokenType::ENDFILE) 
            break;
        Advance();
    }

    return program;
}

void Parser::Advance() {
    t_idx++;
    current_token = tokens[t_idx];
    if(t_idx >= tokens.size()-1)
        peek_token = Token();
    else
        peek_token = tokens[t_idx+1];
}

bool Parser::CurrentTokenIs(TokenType type) const {
    return current_token.type == type;
}
bool Parser::PeekTokenIs(TokenType type) const {
    return peek_token.type == type;
}

bool Parser::DoublePeekIs(TokenType type) const {
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

bool Parser::PeekIsWhitespace() const {
    return PeekTokenIs(TokenType::NEWLINE) || PeekTokenIs(TokenType::INDENT) 
        || PeekTokenIs(TokenType::DEDENT);
}

bool Parser::CurrentIsWhitespace() const {
    return CurrentTokenIs(TokenType::NEWLINE) 
        || CurrentTokenIs(TokenType::INDENT) || CurrentTokenIs(TokenType::DEDENT);
}

bool Parser::IsCurrentEnd() const {
    return CurrentTokenIs(TokenType::NEWLINE) || CurrentTokenIs(TokenType::ENDFILE);
}

bool Parser::IsPeekEnd() const {
    return PeekTokenIs(TokenType::NEWLINE) || PeekTokenIs(TokenType::ENDFILE);
}

PrecLevel Parser::PeekPrecedence() const {
    if(precedences.find(peek_token.type) != precedences.end()) {
        return precedences[peek_token.type];
    }
    return PrecLevel::LOWEST;
}

PrecLevel Parser::CurrentPrecedence() const {
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

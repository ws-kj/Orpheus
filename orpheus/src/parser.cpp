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

std::shared_ptr<Statement> Parser::ParseStatement() {
    switch(current_token.type) { 
        case TokenType::VAR:
            return ParseVarStatement();
        case TokenType::RETURN:
            return ParseReturnStatement();
        case TokenType::FUNC:
            return ParseFunctionStatement();
        case TokenType::PASS:
            return ParsePassStatement();
        case TokenType::NEWLINE:
        case TokenType::INDENT:
        case TokenType::DEDENT:
            return nullptr;
        default:
            return ParseExpressionStatement();
    }
}

std::shared_ptr<ExpressionStatement> Parser::ParseExpressionStatement() {
    std::shared_ptr<Expression> expr = ParseExpression(PrecLevel::LOWEST);
    return std::make_shared<ExpressionStatement>(ExpressionStatement(current_token, expr));
}

std::shared_ptr<VarStatement> Parser::ParseVarStatement() {
    return nullptr;
}

std::shared_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    return nullptr;
}

std::shared_ptr<FunctionStatement> Parser::ParseFunctionStatement() {
    return nullptr;
}

std::shared_ptr<PassStatement> Parser::ParsePassStatement() {
    return nullptr;
}

std::shared_ptr<Expression> Parser::ParseExpression(PrecLevel precedence) {
    if(prefix_fns.find(current_token.type) == prefix_fns.end()) {
        std::stringstream msg;
        msg << "unexpected token " << current_token.type;
        ErrorHandler::Error(current_token.line, msg.str());
        return nullptr;
    }

    auto prefix = prefix_fns[current_token.type];
    std::shared_ptr<Expression> left_exp = (this->*prefix)();

    while(!IsPeekEnd() && precedence < PeekPrecedence()) {
        if(infix_fns.find(peek_token.type) == infix_fns.end()) {
            return left_exp;
        }

        auto infix = infix_fns[peek_token.type];
        Advance();
        left_exp = (this->*infix)(left_exp);
    }

    return left_exp;
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

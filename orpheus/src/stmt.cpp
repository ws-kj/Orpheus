#include "../include/parser.h"

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
        case TokenType::WHILE:
            return ParseWhileStatement();
        case TokenType::NEWLINE:
        case TokenType::INDENT:
        case TokenType::DEDENT:
            return nullptr;
        default:
            return ParseExpressionStatement();
    }
}

std::shared_ptr<ExpressionStatement> Parser::ParseExpressionStatement() {
    std::shared_ptr<Node> expr = ParseExpression(PrecLevel::LOWEST);
    return std::make_shared<ExpressionStatement>(ExpressionStatement(current_token, expr));
}

std::shared_ptr<VarStatement> Parser::ParseVarStatement() {
    Token tok = current_token;
    if(!ExpectPeek(TokenType::IDENTIFIER)) return nullptr;

    std::shared_ptr<Identifier> name = std::make_shared<Identifier>(Identifier(current_token, current_token.literal));

    if(!ExpectPeek(TokenType::COLON)) return nullptr;
    Advance();

    std::shared_ptr<TypeLiteral> annotation = std::dynamic_pointer_cast<TypeLiteral>(ParseTypeLiteral());

    if(!ExpectPeek(TokenType::EQUAL)) return nullptr;
    Advance();

    std::shared_ptr<Node> value = ParseExpression(PrecLevel::LOWEST);
    
    if(IsPeekEnd()) Advance();

    return std::make_shared<VarStatement>(VarStatement(tok, name, annotation, value));

}

std::shared_ptr<ReturnStatement> Parser::ParseReturnStatement() {
    Token tok = current_token;
    Advance();
    std::shared_ptr<Node> ret_value = ParseExpression(PrecLevel::LOWEST);
    if(IsPeekEnd()) Advance();
    return std::make_shared<ReturnStatement>(ReturnStatement(tok, ret_value));
}

std::shared_ptr<FunctionStatement> Parser::ParseFunctionStatement() {
    Token tok = current_token;
    if(!ExpectPeek(TokenType::IDENTIFIER)) return nullptr;
    auto name = std::make_shared<Identifier>(Identifier(current_token, current_token.literal));
    if(!ExpectPeek(TokenType::LPAREN)) return nullptr;
    auto params = ParseFunctionParameters();

    std::shared_ptr<Node> return_type = nullptr;
    if(PeekTokenIs(TokenType::COLON)) {
        Advance();
        Advance();
        return_type = ParseTypeLiteral();
    } else {
        Token typetoken = Token(TokenType::NIL, "nil", current_token.line);
        return_type = std::make_shared<TypeLiteral>(TypeLiteral(typetoken, false));
    }
    std::shared_ptr<Node> body = ParseArrowBlock();
    FunctionStatement literal(tok, name, params, body, return_type);
    return std::make_shared<FunctionStatement>(literal);
}


std::vector<std::pair<std::shared_ptr<Identifier>, std::shared_ptr<TypeLiteral>>> Parser::ParseFunctionParameters() {
    std::vector<std::pair<std::shared_ptr<Identifier>, std::shared_ptr<TypeLiteral>>> params;
    if(PeekTokenIs(TokenType::RPAREN)) {
        Advance();
        return params;
    }
    Advance();
    std::shared_ptr<Identifier> ident = std::make_shared<Identifier>(Identifier(current_token, current_token.literal));
    if(!ExpectPeek(TokenType::COLON)) return params;
    Advance();
    std::shared_ptr<TypeLiteral> annotation = std::dynamic_pointer_cast<TypeLiteral>(ParseTypeLiteral());
    if((*annotation).token.type == TokenType::T_AUTO) {
        ErrorHandler::Error(current_token.line, "auto invalid in function declaration");
        return params;
    }
    params.push_back(std::make_pair(ident, annotation));

    while(PeekTokenIs(TokenType::COMMA)) {
        Advance();
        Advance();
        std::shared_ptr<Identifier> ident = std::make_shared<Identifier>(Identifier(current_token, current_token.literal));
        if(!ExpectPeek(TokenType::COLON)) return params;
        Advance();
        std::shared_ptr<TypeLiteral> annotation = std::dynamic_pointer_cast<TypeLiteral>(ParseTypeLiteral());
        if((*annotation).token.type == TokenType::T_AUTO) {
            ErrorHandler::Error(current_token.line, "auto invalid in function declaration");
            return params;
        }
        params.push_back(std::make_pair(ident, annotation));
    }

    if(!ExpectPeek(TokenType::RPAREN)) return {};
    return params;
}

std::shared_ptr<PassStatement> Parser::ParsePassStatement() {
    return std::make_shared<PassStatement>(PassStatement(current_token));
}

std::shared_ptr<WhileStatement> Parser::ParseWhileStatement() {
    Token tok = current_token;
    Advance();
    std::shared_ptr<Node> condition = ParseExpression(PrecLevel::LOWEST);

    std::shared_ptr<Expression> check_cond = std::dynamic_pointer_cast<Expression>(condition);
    if(!check_cond) {
        ErrorHandler::Error(current_token.line, "expected expression");
        return nullptr;
    }
    std::shared_ptr<Node> body = ParseArrowBlock();
    return std::make_shared<WhileStatement>(WhileStatement(tok, condition, body));
}



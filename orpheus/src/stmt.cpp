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

std::shared_ptr<WhileStatement> Parser::ParseWhileStatement() {
    return nullptr;
}



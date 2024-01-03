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
    return nullptr;
}

std::shared_ptr<FunctionStatement> Parser::ParseFunctionStatement() {
    return nullptr;
}

std::shared_ptr<PassStatement> Parser::ParsePassStatement() {
    return std::make_shared<PassStatement>(PassStatement(current_token));
}

std::shared_ptr<WhileStatement> Parser::ParseWhileStatement() {
    return nullptr;
}



#include "../include/parser.h"
#include <stdexcept>

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

std::shared_ptr<Expression> Parser::ParsePrefixExpression() {
    PrefixExpression expr(current_token, current_token.literal, nullptr);
    Advance();
    expr.right = ParseExpression(PrecLevel::PREFIX);
    return std::make_shared<Expression>(expr);
}

std::shared_ptr<Expression> Parser::ParseInfixExpression(std::shared_ptr<Expression> left) {
    InfixExpression expr(current_token, left, current_token.literal, nullptr);
    PrecLevel prec = CurrentPrecedence();
    Advance();
    expr.right = ParseExpression(prec);
    return std::make_shared<Expression>(expr);
}

std::shared_ptr<Expression> Parser::ParseGroupedExpression() {
    Advance();
    std::shared_ptr<Expression> expr = ParseExpression(PrecLevel::LOWEST);
    if(!ExpectPeek(TokenType::RPAREN)) return nullptr;
    return expr;
}

std::shared_ptr<Expression> Parser::ParseBlockExpression() {
    BlockExpression block(current_token, {});
    Advance();

    while(!CurrentTokenIs(TokenType::DEDENT) && !CurrentTokenIs(TokenType::ENDFILE)) {
        std::shared_ptr<Statement> stmt = ParseStatement();
        if(stmt != nullptr) {
            block.statements.push_back(stmt);
        }
        Advance();
    }

    return std::make_shared<Expression>(block);
}

std::shared_ptr<Expression> Parser::ParseArrowBlock() {
    if(PeekTokenIs(TokenType::ARROW)) {
        Advance();
        if(PeekTokenIs(TokenType::NEWLINE)) {
            return ParseBlockExpression();
        } else {
            Token token = current_token;
            Advance();
            return std::make_shared<Expression>(BlockExpression(token, {ParseStatement()}));
        }
    } else {
        if(!ExpectPeek(TokenType::NEWLINE)) return nullptr;
        if(!ExpectPeek(TokenType::ARROW)) return nullptr;

        if(PeekTokenIs(TokenType::NEWLINE)) {
            return ParseBlockExpression();
        } else {
            Token token = current_token;
            Advance();
            return std::make_shared<Expression>(BlockExpression(token, {ParseStatement()}));
        }
    }
}

std::shared_ptr<Expression> Parser::ParseIdentifier() {
    return nullptr;
}

std::shared_ptr<Expression> Parser::ParseInteger() {
    try {
        int value = std::stoi(current_token.literal);
        return std::make_shared<IntegerLiteral>(IntegerLiteral(current_token, value));
    } catch (const std::invalid_argument& e) {
        ErrorHandler::Error(current_token.line, "could not parse integer literal");
        return nullptr;
    } catch (const std::out_of_range& e) {
        ErrorHandler::Error(current_token.line, "integer literal out of range");
        return nullptr;
    }
}
    

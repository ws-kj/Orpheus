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
    return std::make_shared<PrefixExpression>(expr);
}

std::shared_ptr<Expression> Parser::ParseInfixExpression(std::shared_ptr<Expression> left) {
    InfixExpression expr(current_token, left, current_token.literal, nullptr);
    PrecLevel prec = CurrentPrecedence();
    Advance();
    expr.right = ParseExpression(prec);
    return std::make_shared<InfixExpression>(expr);
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

    return std::make_shared<BlockExpression>(block);
}

std::shared_ptr<Expression> Parser::ParseArrowBlock() {
    if(PeekTokenIs(TokenType::ARROW)) {
        Advance();
        if(PeekTokenIs(TokenType::NEWLINE)) {
            return ParseBlockExpression();
        } else {
            Token token = current_token;
            Advance();
            return std::make_shared<BlockExpression>(BlockExpression(token, {ParseStatement()}));
        }
    } else {
        if(!ExpectPeek(TokenType::NEWLINE)) return nullptr;
        if(!ExpectPeek(TokenType::ARROW)) return nullptr;

        if(PeekTokenIs(TokenType::NEWLINE)) {
            return ParseBlockExpression();
        } else {
            Token token = current_token;
            Advance();
            return std::make_shared<BlockExpression>(BlockExpression(token, {ParseStatement()}));
        }
    }
}

std::shared_ptr<Expression> Parser::ParseIdentifier() {
/*    if(PeekTokenIs(TokenType::EQUAL)) {
        Token tok = current_token;
        std::shared_ptr<Identifier> name = std::make_shared<Identifier>(Identifier(tok, current_token.literal));
        Advance();
        Advance();
        std::shared_ptr<Expression> value = ParseExpression(PrecLevel::LOWEST);
        AssignmentStatement stmt = AssignmentStatement(tok, name, value);
        return std::make_shared<AssignmentStatement>(stmt);
    } */ 
    return std::make_shared<Identifier>(Identifier(current_token, current_token.literal));   
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

std::shared_ptr<Expression> Parser::ParseFloat() {
    try {
        float value = std::stof(current_token.literal);
        return std::make_shared<FloatLiteral>(FloatLiteral(current_token, value));
    } catch (const std::invalid_argument& e) {
        ErrorHandler::Error(current_token.line, "could not parse float literal");
        return nullptr;
    } catch (const std::out_of_range& e) {
        ErrorHandler::Error(current_token.line, "float literal out of range");
        return nullptr;
    }
}

std::shared_ptr<Expression> Parser::ParseString() {
    return std::make_shared<StringLiteral>(StringLiteral(current_token, current_token.literal));   
}

std::shared_ptr<Expression> Parser::ParseBoolean() {
    return std::make_shared<Boolean>(Boolean(current_token, CurrentTokenIs(TokenType::TRUE)));
}

std::shared_ptr<Expression> Parser::ParseNil() {
    return std::make_shared<Nil>(Nil(current_token));
}

std::vector<std::shared_ptr<Expression>> Parser::ParseExpressionList(TokenType end) {
    std::vector<std::shared_ptr<Expression>> list;
    IgnoreWhitespace();

    if(PeekTokenIs(end)) {
        Advance();
        return list;
    }

    Advance();
    list.push_back(ParseExpression(PrecLevel::LOWEST));

    while(PeekTokenIs(TokenType::COMMA)) {
        Advance();
        IgnoreWhitespace();
        Advance();
        list.push_back(ParseExpression(PrecLevel::LOWEST));
        IgnoreWhitespace();
    }

    if(!ExpectPeek(end)) {
        ErrorHandler::Error(current_token.line, "expected end token in expression list");
        std::vector<std::shared_ptr<Expression>> empty;
        return empty;
    }

    return list;
}

std::shared_ptr<Expression> Parser::ParseListLiteral() {
    return std::make_shared<ListLiteral>(ListLiteral(current_token, ParseExpressionList(TokenType::RBRACKET)));
}

std::shared_ptr<Expression> Parser::ParseMapLiteral() {
    Token tok = current_token;

    std::map<std::shared_ptr<Expression>, std::shared_ptr<Expression>> pairs;

    while(!PeekTokenIs(TokenType::RBRACE)) {
        IgnoreWhitespace();
        Advance();

        std::shared_ptr<Expression> key = ParseExpression(PrecLevel::LOWEST);
        const std::type_info& tid = typeid(*key);
        if (tid != typeid(IntegerLiteral) &&
            tid != typeid(StringLiteral) &&
            tid != typeid(FloatLiteral) &&
            tid != typeid(Boolean)) {
            ErrorHandler::Error(current_token.line, "invalid type for map key");
            return nullptr;
        }

        if(!ExpectPeek(TokenType::COLON)) {
            ErrorHandler::Error(current_token.line, "expected colon in map literal");
            return nullptr;
        }

        IgnoreWhitespace();
        Advance();
        std::shared_ptr<Expression> value = ParseExpression(PrecLevel::LOWEST);
        pairs[key] = value;

        IgnoreWhitespace();
        if(!PeekTokenIs(TokenType::RBRACE)) {
            if(!ExpectPeek(TokenType::COMMA)) {
                ErrorHandler::Error(current_token.line, "expected comma in map literal");
                return nullptr;
            }
        }
    }

    if(!ExpectPeek(TokenType::RBRACE)) {
        ErrorHandler::Error(current_token.line, "expected right brace in map literal");
    }

    return std::make_shared<MapLiteral>(MapLiteral(tok, pairs));
}

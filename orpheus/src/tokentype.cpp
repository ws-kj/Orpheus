#include "../include/tokentype.h"

TokenType type_literals[] = {
    TokenType::T_INT,
    TokenType::T_FLOAT,
    TokenType::T_STR,
    TokenType::T_BOOL,
    TokenType::T_ANY,
    TokenType::T_AUTO,
    TokenType::T_LIST,
    TokenType::FUNC,
};

std::ostream& operator<<(std::ostream& out, const TokenType value) {
    const char* str = "UNKNOWN TOKEN";
    #define PROCESS_VAL(p) case (p): str = #p; break;
    switch(value) {
        PROCESS_VAL(TokenType::ENDFILE);
        PROCESS_VAL(TokenType::INDENT);
        PROCESS_VAL(TokenType::DEDENT);
        PROCESS_VAL(TokenType::NEWLINE);
        PROCESS_VAL(TokenType::WHITESPACE);
        PROCESS_VAL(TokenType::IDENTIFIER);
        PROCESS_VAL(TokenType::STRING);
        PROCESS_VAL(TokenType::INTEGER);
        PROCESS_VAL(TokenType::FLOAT);
        PROCESS_VAL(TokenType::NIL);
        PROCESS_VAL(TokenType::LPAREN);
        PROCESS_VAL(TokenType::RPAREN);
        PROCESS_VAL(TokenType::LBRACE);
        PROCESS_VAL(TokenType::RBRACE);
        PROCESS_VAL(TokenType::LBRACKET);
        PROCESS_VAL(TokenType::RBRACKET);
        PROCESS_VAL(TokenType::MINUS);
        PROCESS_VAL(TokenType::PLUS);
        PROCESS_VAL(TokenType::SEMICOLON);
        PROCESS_VAL(TokenType::STAR);
        PROCESS_VAL(TokenType::COMMA);
        PROCESS_VAL(TokenType::DOT);
        PROCESS_VAL(TokenType::HASH);
        PROCESS_VAL(TokenType::SLASH);
        PROCESS_VAL(TokenType::PERCENT);
        PROCESS_VAL(TokenType::BANG);
        PROCESS_VAL(TokenType::BANG_EQUAL);
        PROCESS_VAL(TokenType::EQUAL);
        PROCESS_VAL(TokenType::EQUAL_EQUAL);
        PROCESS_VAL(TokenType::LESS);
        PROCESS_VAL(TokenType::LESS_EQUAL);
        PROCESS_VAL(TokenType::GREATER);
        PROCESS_VAL(TokenType::GREATER_EQUAL);
        PROCESS_VAL(TokenType::COLON);
        PROCESS_VAL(TokenType::ARROW);
        PROCESS_VAL(TokenType::FUNC);
        PROCESS_VAL(TokenType::T_INT);
        PROCESS_VAL(TokenType::T_STR);
        PROCESS_VAL(TokenType::T_FLOAT);
        PROCESS_VAL(TokenType::T_BOOL);
        PROCESS_VAL(TokenType::T_AUTO);
        PROCESS_VAL(TokenType::T_ANY);
        PROCESS_VAL(TokenType::T_LIST);
        PROCESS_VAL(TokenType::T_MAP);
        PROCESS_VAL(TokenType::QUESTION);
        PROCESS_VAL(TokenType::AND);
        PROCESS_VAL(TokenType::OR);
        PROCESS_VAL(TokenType::NOT);
        PROCESS_VAL(TokenType::TRUE);
        PROCESS_VAL(TokenType::FALSE);
        PROCESS_VAL(TokenType::IF);
        PROCESS_VAL(TokenType::ELSE);
        PROCESS_VAL(TokenType::WHILE);
        PROCESS_VAL(TokenType::VAR);
        PROCESS_VAL(TokenType::RETURN);
        PROCESS_VAL(TokenType::PASS);
        PROCESS_VAL(TokenType::IS);
        PROCESS_VAL(TokenType::INSTRUCT);
        PROCESS_VAL(TokenType::COLON_COLON);
        PROCESS_VAL(TokenType::LESS_LESS);
        PROCESS_VAL(TokenType::GREATER_GREATER);
    }
    #undef PROCESS_VAL
    return out << str;
}

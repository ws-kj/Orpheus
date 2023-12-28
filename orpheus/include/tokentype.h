#pragma once

#include <string>
#include <iostream>
#include <ostream>
#include <type_traits>

enum class TokenType 
{
    ENDFILE,
    INDENT,
    DEDENT,
    NEWLINE,
    WHITESPACE,

    IDENTIFIER,
    
    STRING,
    INTEGER,
    FLOAT,
    NIL,
    
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    MINUS,
    PLUS,
    SEMICOLON,
    STAR,
    COMMA,
    DOT,
    HASH,
    SLASH,
    PERCENT,

    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    COLON,

    
    ARROW,

    FUNC,
    T_INT,
    T_STR,
    T_FLOAT,
    T_BOOL,
    T_AUTO,
    T_ANY,
    T_LIST,
    T_MAP,
    QUESTION,

    AND,
    OR,
    NOT,
    TRUE,
    FALSE,
    IF,
    ELSE,
    WHILE,
    VAR,
    RETURN,
    PASS,
    IS,
  
    INSTRUCT,
    COLON_COLON,
    LESS_LESS,
    GREATER_GREATER,
};

std::ostream& operator<<(std::ostream& out, const TokenType value);

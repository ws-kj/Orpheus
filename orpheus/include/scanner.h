#pragma once

#include <vector>
#include <cctype>
#include <unordered_map>

#include "token.h"
#include "error_handler.h"

class Scanner {
    std::string source;
    std::vector<Token> tokens;

    int start, current, line;

    int level;
    std::vector<int> levels;

public:
    Scanner(const std::string& source)
        : source(source), tokens(), start(0), current(0), line(0), level(0), levels() 
        {}

    std::vector<Token> ScanTokens();

    void ScanToken();
    void AddToken(TokenType token_type, std::string literal="");

    bool Match(char expected);
    bool IsAtEnd() const;
    char Peek() const;
    char PeekNext() const;
    char Advance();

    void Indent();
    void Identifier();
    void Number();
    void String();

    //    std::vector<Token&> DoublePeek();
};

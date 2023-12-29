#include "../include/scanner.h"

std::unordered_map<std::string, TokenType> keywords = {
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"var", TokenType::VAR},
    {"return", TokenType::RETURN},
    {"pass", TokenType::PASS},
    {"func", TokenType::FUNC},
    {"is", TokenType::IS},
    {"nil", TokenType::NIL},
    {"str", TokenType::T_STR},
    {"int", TokenType::T_INT},
    {"float", TokenType::T_FLOAT},
    {"bool", TokenType::T_BOOL},
    {"any", TokenType::T_ANY},
    {"auto", TokenType::T_AUTO},
    {"list", TokenType::T_LIST},
    {"map", TokenType::T_MAP}
};

std::vector<Token> Scanner::ScanTokens() {
    while(!IsAtEnd()) {
        start = current;
        ScanToken();
    }
    tokens.push_back(Token(TokenType::ENDFILE, "", line)); 
    return tokens;
}

bool Scanner::IsAtEnd() const {
    return current >= source.length();
}

void Scanner::ScanToken() {
    char c = Advance();
    switch (c) {
        case '(':
            AddToken(TokenType::LPAREN);
            break;
        case ')':
            AddToken(TokenType::RPAREN);
            break;
        case '{':
            AddToken(TokenType::LBRACE);
            break;
        case '}':
            AddToken(TokenType::RBRACE);
            break;
        case '[':
            AddToken(TokenType::LBRACKET);
            break;
        case ']':
            AddToken(TokenType::RBRACKET);
            break;
        case '-':
            AddToken(Match('>') ? TokenType::ARROW : TokenType::MINUS);
            break;
        case '+':
            AddToken(TokenType::PLUS);
            break;
        case ';':
            AddToken(TokenType::SEMICOLON);
            break;
        case '*':
            AddToken(TokenType::STAR);
            break;
        case '/':
            if (Match('/')) {
                while (Peek() != '\n' && !IsAtEnd()) {
                    Advance();
                }
            } else {
                AddToken(TokenType::SLASH);
            }
            break;
        case '%':
            AddToken(TokenType::PERCENT);
            break;
        case ',':
            AddToken(TokenType::COMMA);
            break;
        case '.':
            AddToken(TokenType::DOT);
            break;
        case '!':
            AddToken(Match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            AddToken(Match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case ':':
            AddToken(Match(':') ? TokenType::COLON_COLON : TokenType::COLON);
            break;
        case '<':
            if (Match('=')) {
                AddToken(TokenType::LESS_EQUAL);
            } else {
                AddToken(TokenType::LESS);
            }
            break;
        case '>':
            if (Match('=')) {
                AddToken(TokenType::GREATER_EQUAL);
            } else {
                AddToken(TokenType::GREATER);
            }
            break;
        case '?':
            AddToken(TokenType::QUESTION);
            break;
        case '\n':
            AddToken(TokenType::NEWLINE);
            line++;
            Indent();
            break;
        case '"':
            String();
            break;
        default:
            if (std::isdigit(c)) {
                Number();
            } else if (std::isalpha(c) || c == '_') {
                Identifier();
            } else if (c == ' ' || c == '\t' || c == '\r') {
                // Ignore
            } else {
                ErrorHandler::Error(line, "unexpected character");
            }
            break;
    }
}

void Scanner::AddToken(TokenType token_type, std::string literal) {
    std::string text = source.substr(start, current-start);
    tokens.push_back(Token(token_type, literal.empty() ? text : literal, line));
}

bool Scanner::Match(char expected) {
    if(IsAtEnd()) return false;
    if(source[current] != expected) return false;
    
    current++;
    return true;
}

char Scanner::Peek() const {
    if(IsAtEnd()) return '\0';
    return source[current];
}

char Scanner::PeekNext() const {
    if(current + 1 >= source.length()) return '\0';
    return source[current+1];
}

char Scanner::Advance() {
    current++;
    return source[current-1];
}

void Scanner::Indent() {
    int n = 0;
    while(Peek() == ' ' && !IsAtEnd()) {
        n++;
        Advance();
    }

    if(Peek() == '\n') return;

    if(n > level) {
        AddToken(TokenType::INDENT);
        levels.push_back(n);
        level = n;
    }

    while(n < level) {
        AddToken(TokenType::DEDENT);
        if(levels.empty()) {
            level = 0;
            break;
        } else {
            levels.pop_back();
            if(levels.empty()) {
                level = 0;
                break;
            }
            level = levels.back();
        }

        if(level < n) ErrorHandler::Error(line, "invalid indentation");
    }
}
       
void Scanner::Identifier() {
    std::string text = "";

    while(std::isalpha(Peek()) || Peek() == '_' || std::isdigit(Peek())) {
        Advance();
        text = source.substr(start, current-start);
    }
    
    TokenType type;
    if(keywords.find(text) != keywords.end()) {
        type = keywords[text];
    } else {
        type = TokenType::IDENTIFIER;
    }

    AddToken(type, text);
}

void Scanner::Number() {
    bool isfloat = false;
    
    while(std::isdigit(Peek())) {
        Advance();
    }

    if(Peek() == '.' && std::isdigit(PeekNext())) {
        isfloat = true;
        Advance();

        while(std::isdigit(Peek())) {
            Advance();
        }
    }

    if(isfloat) {
        AddToken(TokenType::FLOAT, source.substr(start, current-start));
    } else {
        AddToken(TokenType::INTEGER, source.substr(start, current-start));
    }
}

void Scanner::String() {
    while(Peek() != '"' && !IsAtEnd()) {
        if(Peek() == '\n') {
            line++;
        }
        Advance();
    }

    if(IsAtEnd()) {
        ErrorHandler::Error(line, "unterminated string");
        return;
    }

    Advance();

    std::string value = source.substr(start+1, current - start - 2);
    AddToken(TokenType::STRING, value);
}

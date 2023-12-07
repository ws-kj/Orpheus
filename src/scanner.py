from tokentype import TokenType
from tokens import Token
from error_handler import ErrorHandler

class Scanner(object):

    keywords = {
        "and": TokenType.AND,
        "or": TokenType.OR,
        "not": TokenType.NOT,
        "true": TokenType.TRUE,
        "false": TokenType.FALSE,
        "if": TokenType.IF,
        "else": TokenType.ELSE,
        "for": TokenType.FOR,
        "while": TokenType.WHILE,
        "var": TokenType.VAR,
        "ret": TokenType.RET,
        "pass": TokenType.PASS,
        "break": TokenType.BREAK,
        "loop": TokenType.LOOP,

        #orpheus
        "model": TokenType.MODEL,
        "tool": TokenType.TOOL,
        "action": TokenType.ACTION,
        "spawn": TokenType.SPAWN,
        "num": TokenType.T_NUM,
        "str": TokenType.T_STR,
        "nil": TokenType.T_NIL,
        "fn": TokenType.FN
    }

    def __init__(self, source: str):
        self.source = source
        self.tokens = []

        self.start = 0
        self.current = 0
        self.line = 1

        self.level = 0
        self.levels = []

    def scan_tokens(self):
        while(not self.is_at_end()):
            self.start = self.current
            self.scan_token()

        self.tokens.append(Token(TokenType.EOF, "", None, self.line))
        return self.tokens

    def is_at_end(self):
        return self.current >= len(self.source)

    def scan_token(self):
        c = self.advance()

        match c:
            case '(':
                self.add_token(TokenType.LPAREN)
            case ')':
                self.add_token(TokenType.RPAREN)
            case '{':
                self.add_token(TokenType.LBRACE)
            case '}':
                self.add_token(TokenType.RBRACE)
            case '[':
                self.add_token(TokenType.LBRACKET)
            case ']':
                self.add_token(TokenType.RBRACKET)
            case '-':
                self.add_token(TokenType.ARROW if self.match('>') else TokenType.MINUS)
            case '+':
                self.add_token(TokenType.PLUS)
            case ';':
                self.add_token(TokenType.SEMICOLON)
            case '*':
                self.add_token(TokenType.STAR)
            case '/':
                self.add_token(TokenType.SLASH)
            case '%':
                self.add_token(TokenType.PERCENT)
            case ',':
                self.add_token(TokenType.COMMA)
            case '.':
                self.add_token(TokenType.DOT)
            case '!':
                self.add_token(TokenType.BANG_EQUAL if self.match('=') else TokenType.BANG)
            case '=':
                self.add_token(TokenType.EQUAL_EQUAL if self.match('=') else TokenType.EQUAL)
            case ':':
                self.add_token(TokenType.COLON_COLON if self.match(':') else TokenType.COLON)
            case '<':
                if self.match('='):
                    self.add_token(TokenType.LESS_EQUAL)
                elif self.match('<'):
                    self.instruction()
                else:
                    self.add_token(TokenType.LESS)
            case '>':
                if self.match('='):
                    self.add_token(TokenType.GREATER_EQUAL)
                elif self.match('>'):
                    #self.add_token(TokenType.GREATER_GREATER)
                    pass
                else:
                    self.add_token(TokenType.GREATER)
            case '#':
                while(self.peek() != '\n' and not self.is_at_end()):
                    self.advance()
            case '\n':
                self.add_token(TokenType.NEWLINE)
                self.line += 1
                self.indent()
            case '"':
                self.string()
            case _:
                if(c.isdigit()):
                    self.number()
                elif(c.isalpha() or c == '_'):
                    self.identifier()
                elif(c == ' ' or c == '\t' or c == '\r'):
                    pass
                else:
                    ErrorHandler.error(self.line, "Unexpected character.")

    def indent(self):
        n = 0
        while(self.peek() == ' ' and not self.is_at_end()):
            n += 1
            self.advance()

        if(self.peek() == '\n' or self.peek() == '#'):
            return
        if(n > self.level):
            self.add_token(TokenType.INDENT) 
            self.levels.append(n)
            self.level = n
        
        while(n < self.level):
            self.add_token(TokenType.DEDENT)

            if(self.levels == []):
                self.level = 0
                break
            else:
                self.levels.pop()
                if(self.levels == []):
                    self.level = 0
                    break
                self.level = self.levels[len(self.levels)-1]

            if(self.level < n):
                ErrorHandler.error(self.line, "Invalid indentation.")

    def identifier(self):
        text = ""
        while(self.peek().isalpha() or self.peek() == '_' or self.peek().isdigit()):
            self.advance()
            text = self.source[self.start:self.current]

        if text in self.keywords:
            token_type = self.keywords[text]
        else:
            token_type = TokenType.IDENTIFIER

        self.add_token(token_type)

    def number(self):
        while(self.peek().isdigit()):
            self.advance()

        if(self.peek() == '.' and self.peek_next().isdigit()):
            self.advance()

            while(self.peek().isdigit()):
                self.advance()

        self.add_token(TokenType.NUMBER, float(self.source[self.start:self.current]))

    def string(self):
        while(self.peek() != '"' and not self.is_at_end()):
            if(self.peek() == '\n'):
                self.line += 1
            self.advance()
        if(self.is_at_end()):
            ErrorHandler.error(self.line, "Unterminated string.")
            return

        self.advance()

        value = self.source[self.start+1:self.current-1]
        self.add_token(TokenType.STRING, value)

    def instruction(self):
        while(self.double_peek() != ">>" and not self.is_at_end()):
            if(self.peek() == '\n'):
                self.line += 1
            self.advance()
        if(self.is_at_end()):
            ErrorHandler.error(self.line, "Untermined instruction.")
            return
        self.advance()

        value = self.source[self.start+2:self.current-2]
        self.add_token(TokenType.INSTRUCT, value)

    def match(self, expected: str):
        if(self.is_at_end()):
            return False

        if(self.source[self.current] != expected):
            return False

        self.current += 1
        return True

    def peek(self):
        if (self.is_at_end()):
            return '\0'
        return self.source[self.current]

    def double_peek(self):
        if(self.is_at_end() or self.current >= len(self.source)):
            return '\0'
        return self.source[self.current-1:self.current+1]

    def peek_next(self):
        if(self.current + 1 >= len(self.source)):
            return '\0'
        return self.source[self.current+1]

    def advance(self):
        self.current += 1
        return self.source[self.current-1]

    def add_token(self, token_type: TokenType, literal: object = None):
        text = self.source[self.start:self.current]
        self.tokens.append(Token(token_type, text, literal, self.line))

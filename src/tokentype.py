from enum import Enum, auto

class TokenType(Enum):
    EOF = auto()
    INDENT = auto()
    DEDENT = auto()
    NEWLINE = auto()
    WHITESPACE = auto()

    # literals
    IDENTIFIER = auto()
    STRING = auto()
    NUMBER = auto()
    INSTRUCT = auto()

    # single char tokens
    LPAREN = auto()
    RPAREN = auto()
    LBRACE = auto()
    RBRACE = auto()
    LBRACKET = auto()
    RBRACKET = auto()
    MINUS = auto()
    PLUS = auto()
    SEMICOLON = auto()
    STAR = auto()
    COMMA = auto()
    DOT = auto()
    HASH = auto()
    SLASH = auto()
    PERCENT = auto()

    # 1 or 2 char tokens
    BANG = auto()
    BANG_EQUAL = auto()
    EQUAL = auto()
    EQUAL_EQUAL = auto()
    LESS = auto()
    LESS_EQUAL = auto()
    GREATER = auto()
    GREATER_EQUAL = auto()
    COLON = auto()

    # orpheus specific tokens
    COLON_COLON = auto()    # ::
    LESS_LESS = auto() # <<
    GREATER_GREATER = auto() # >>
    ARROW = auto()  # ->

    # orpheus keywords
    MODEL = auto()
    TOOL = auto()
    ACTION = auto()
    SPAWN = auto()
    FN = auto() 
    T_NUM = auto()
    T_STR = auto()
    T_NIL = auto()

    # keywords
    AND = auto()
    OR = auto()
    NOT = auto()
    TRUE = auto()
    FALSE = auto()
    IF = auto()
    ELSE = auto()
    FOR = auto()
    WHILE = auto()
    VAR = auto()
    RET = auto()
    PASS = auto()
    BREAK = auto()
    LOOP = auto()

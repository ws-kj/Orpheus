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
    INSTRUCT = auto()

    INTEGER = auto()
    FLOAT = auto()

    # single char tokens
    LPAREN = auto()
    RPAREN = auto()
    LBRACE = auto()
    RBRACE = auto()
    LBRACKET = auto()
    RBRACKET = auto()
    MINUS = auto()
    PLUS = auto()
    SEMICOLON = auto() # obsolete?
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
    MODEL = auto() # todo
    TOOL = auto() # todo
    ACTION = auto() # todo
    SPAWN = auto() # todo 
    FUNC = auto()

    # type annotation stuff
    T_INT = auto() 
    T_STR = auto() 
    T_FLOAT = auto()
    T_BOOL = auto()
    T_AUTO = auto()
    T_ANY = auto()
    T_ARRAY = auto()
    T_MAP = auto()
    QUESTION = auto() # for maybe

    # keywords
    AND = auto() # todo
    OR = auto() # todo
    NOT = auto()
    TRUE = auto()
    FALSE = auto()
    IF = auto()
    ELSE = auto() # todo
    FOR = auto() # todo 
    WHILE = auto() # todo
    VAR = auto()
    RETURN = auto()
    PASS = auto() # todo
    BREAK = auto() #todo 
    LOOP = auto() #todo

type_tokens = [
    TokenType.T_INT,
    TokenType.T_FLOAT,
    TokenType.T_STR,
    TokenType.T_BOOL,
    TokenType.T_ANY,
    TokenType.T_AUTO,
    TokenType.T_ARRAY,
    TokenType.T_MAP,
    TokenType.FUNC
]


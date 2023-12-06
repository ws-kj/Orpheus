import tree 
from tokens import Token
from tokentype import TokenType
from error_handler import ErrorHandler
from enum import Enum, IntEnum, auto

class PrecLevel(IntEnum):
    LOWEST = auto()
    EQUALS = auto()
    LESS_GREATER = auto()
    SUM = auto()
    PRODUCT = auto()
    PREFIX = auto()
    CALL = auto()

class Parser(object):

    precedences = {
        TokenType.EQUAL_EQUAL: PrecLevel.EQUALS,
        TokenType.BANG_EQUAL: PrecLevel.EQUALS,
        TokenType.LESS: PrecLevel.LESS_GREATER,
        TokenType.LESS_EQUAL: PrecLevel.LESS_GREATER,
        TokenType.GREATER: PrecLevel.LESS_GREATER,
        TokenType.GREATER_EQUAL: PrecLevel.LESS_GREATER,
        TokenType.PLUS: PrecLevel.SUM,
        TokenType.MINUS: PrecLevel.SUM,
        TokenType.SLASH: PrecLevel.PRODUCT,
        TokenType.STAR: PrecLevel.PRODUCT
    }

    def __init__(self, tokens=None):
        self.initialized = False

        self.prefix_fns = {}
        self.infix_fns = {}

        self.register_all()

        if(tokens != None):
            self.parse_first(tokens)
        else:
            self.tokens=[]

    def register_all(self):
        self.register_prefix(TokenType.IDENTIFIER, self.parse_identifier)
        self.register_prefix(TokenType.NUMBER, self.parse_number)
       
        self.register_prefix(TokenType.NOT, self.parse_prefix_expression)
        self.register_prefix(TokenType.MINUS, self.parse_prefix_expression)

        self.register_prefix(TokenType.TRUE, self.parse_boolean)
        self.register_prefix(TokenType.FALSE, self.parse_boolean)

        self.register_prefix(TokenType.LPAREN, self.parse_grouped_expression)
        self.register_prefix(TokenType.ARROW, self.parse_block_statement)
        self.register_prefix(TokenType.IF, self.parse_if_expression)

        self.register_infix(TokenType.EQUAL_EQUAL, self.parse_infix_expression)
        self.register_infix(TokenType.BANG_EQUAL, self.parse_infix_expression)
        self.register_infix(TokenType.LESS, self.parse_infix_expression)
        self.register_infix(TokenType.LESS_EQUAL, self.parse_infix_expression)
        self.register_infix(TokenType.GREATER, self.parse_infix_expression)
        self.register_infix(TokenType.GREATER_EQUAL, self.parse_infix_expression)
        self.register_infix(TokenType.PLUS, self.parse_infix_expression)
        self.register_infix(TokenType.MINUS, self.parse_infix_expression)
        self.register_infix(TokenType.SLASH, self.parse_infix_expression)
        self.register_infix(TokenType.STAR, self.parse_infix_expression)

    def parse_first(self, tokens):
        self.initialized = True

        self.tokens = tokens

        self.t_idx = 0
        self.current_token = self.tokens[0]
        self.peek_token = self.tokens[1]

    def register_prefix(self, token_type, fn):
        self.prefix_fns[token_type] = fn

    def register_infix(self, token_type, fn):
        self.infix_fns[token_type] = fn


    def advance(self):
        self.t_idx += 1

        self.current_token = self.tokens[self.t_idx] 
        if(self.t_idx >= len(self.tokens)-1):
            self.peek_token = None
        else:
            self.peek_token = self.tokens[self.t_idx+1]

    def parse_program(self):
        if(not self.initialized):
            if(self.tokens != []):
                self.parse_first(self.tokens)
            else:
                return None

        program = tree.Program()

        while(self.current_token.type != TokenType.EOF):
            statement = self.parse_statement()
            if(statement != None):
                program.statements.append(statement)

            self.advance()

        return program

    def parse_statement(self):
        match self.current_token.type:
            case TokenType.VAR:
                return self.parse_var_statement()
            case TokenType.RETURN:
                return self.parse_return_statement()
            case TokenType.MODEL:
                return self.parse_model_statement()
            case TokenType.NEWLINE | TokenType.INDENT | TokenType.DEDENT:
                return None
            case _:
                return self.parse_expression_statement()

    def parse_expression_statement(self):
        statement = tree.ExpressionStatement(self.current_token, None)

        statement.expression = self.parse_expression(PrecLevel.LOWEST)

        return statement

    def parse_expression(self, precedence):
        if not self.current_token.type in self.prefix_fns:
            ErrorHandler.error(
                self.current_token.line, 
                f'No prefix parse for token type {self.current_token.type}')
            return None

        prefix = self.prefix_fns[self.current_token.type]

        left_exp = prefix()

        while(not self.is_peek_end() and precedence < self.peek_precedence()):
            if not self.peek_token.type in self.infix_fns:
                return left_exp
            infix = self.infix_fns[self.peek_token.type]

            self.advance()
            left_exp = infix(left_exp)

        return left_exp

    def parse_identifier(self):
        return tree.Identifier(self.current_token, self.current_token.literal)

    def parse_number(self):
        literal = tree.NumLiteral(self.current_token, None)
        
        try:
            value = int(self.current_token.literal)
            literal.value = value
            return literal
        except ValueError:
            ErrorManager.error(
                self.current_token.line, f'Could not parse integer literal from {literal}')
            return None
      
    def parse_boolean(self):
        return tree.Boolean(self.current_token, self.current_token_is(TokenType.TRUE))

    def parse_grouped_expression(self):
        self.advance()

        exp = self.parse_expression(PrecLevel.LOWEST)

        if not self.expect_peek(TokenType.RPAREN):
            return None

        return exp

    def parse_if_expression(self):
        expression = tree.IfExpression(self.current_token, None, None, None)

        self.advance()
        expression.condition = self.parse_expression(PrecLevel.LOWEST)

        if(self.peek_token.type == TokenType.ARROW):
            self.advance()
            if(self.peek_token.type == TokenType.NEWLINE):
                expression.consequence = self.parse_block_statement()
            else:
                self.advance()
                expression.consequence = self.parse_expression_statement()
        else:
            if(not self.expect_peek(TokenType.NEWLINE)):
                return None
            if(not self.expect_peek(TokenType.ARROW)):
                return None
            print(self.peek_token)
            if(self.peek_token.type == TokenType.NEWLINE):
                expression.consequence = self.parse_block_statement()
            else:
                self.advance()
                expression.consequence = self.parse_expression_statement()

        return expression

    def parse_block_statement(self):
        block = tree.BlockStatement(self.current_token, [])

        self.advance()

        while(not self.current_token_is(TokenType.DEDENT) and not self.current_token_is(TokenType.EOF)):
            statement = self.parse_statement()
            if(statement is not None):
                block.statements.append(statement)

            self.advance()
        return block

    def parse_prefix_expression(self):
        expression = tree.PrefixExpression(
            self.current_token, self.current_token.literal, None)
        
        self.advance()
        expression.right = self.parse_expression(PrecLevel.PREFIX)
        return expression

    def parse_infix_expression(self, left):
        expression = tree.InfixExpression(
            self.current_token, left, self.current_token.lexeme, None)


        prec = self.current_precedence()
        self.advance()
        expression.right = self.parse_expression(prec)

        return expression

    #TODO implement
    def parse_var_statement(self):
        statement = tree.VarStatement(self.current_token, None, None)

        if(not self.expect_peek(TokenType.IDENTIFIER)):
            return None

        statement.name = tree.Identifier(self.current_token, self.current_token.literal)

        while(not self.is_current_end()):
            self.advance()

        return statement

    #TODO implement
    def parse_return_statement(self):
        statement = tree.ReturnStatement(self.current_token, None)

        while(not self.is_current_end()):
            self.advance()

        return statement

    #TODO implement
    def parse_model_statement(self):
        statement = tree.ModelStatement(self.current_token, None)

        if(not self.expect_peek(TokenType.IDENTIFIER)):
            return None

        statement.name = tree.Identifier(self.current_token, self.current_token.literal)

        while(not self.is_current_end()):
            self.advance()

        return statement

    #TODO implement
    def parse_spawn_statement(self):
        statement = tree.SpawnStatement(self.current_token, None)

        if(not self.expect_peek(TokenType.IDENTIFIER)):
            return None

        statement.name = tree.Identifier(self.current_token, self.current_token.literal)

        while(not self.is_current_end()):
            self.advance()

        return statement

    def current_token_is(self, token_type):
        return self.current_token.type == token_type

    def peek_token_is(self, token_type):
        return self.peek_token.type == token_type

    def expect_peek(self, token_type):
        if(self.peek_token_is(token_type)):
            self.advance()
            return True
        ErrorHandler.error(
            self.peek_token.line, f'Expected {token_type}, got {self.peek_token.type}') 
        return False

    def is_current_end(self):
        return self.current_token_is(TokenType.NEWLINE) or self.current_token_is(TokenType.EOF)

    def is_peek_end(self):
        return self.peek_token_is(TokenType.NEWLINE) or self.peek_token_is(TokenType.EOF)

    def peek_precedence(self):
        if(self.peek_token.type in self.precedences):
            return self.precedences[self.peek_token.type]
        return PrecLevel.LOWEST

    def current_precedence(self):
        if(self.current_token.type in self.precedences):
            return self.precedences[self.current_token.type]
        return PrecLevel.LOWEST


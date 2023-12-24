import tree 
from tokens import Token
from tokentype import TokenType, type_tokens
from error_handler import ErrorHandler
from enum import Enum, IntEnum, auto

class PrecLevel(IntEnum):
    LOWEST = auto()
    AND_OR = auto()
    EQUALS = auto()
    LESS_GREATER = auto()
    SUM = auto()
    PRODUCT = auto()
    PREFIX = auto()
    CALL = auto()
    INDEX = auto()

class Parser(object):

    precedences = {
        TokenType.AND: PrecLevel.AND_OR,
        TokenType.OR: PrecLevel.AND_OR,
        TokenType.EQUAL_EQUAL: PrecLevel.EQUALS,
        TokenType.BANG_EQUAL: PrecLevel.EQUALS,
        TokenType.LESS: PrecLevel.LESS_GREATER,
        TokenType.LESS_EQUAL: PrecLevel.LESS_GREATER,
        TokenType.GREATER: PrecLevel.LESS_GREATER,
        TokenType.GREATER_EQUAL: PrecLevel.LESS_GREATER,
        TokenType.PLUS: PrecLevel.SUM,
        TokenType.MINUS: PrecLevel.SUM,
        TokenType.SLASH: PrecLevel.PRODUCT,
        TokenType.STAR: PrecLevel.PRODUCT,
        TokenType.PERCENT: PrecLevel.PRODUCT,
        TokenType.LPAREN: PrecLevel.CALL,
        TokenType.LBRACKET: PrecLevel.INDEX
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

        self.register_prefix(TokenType.INTEGER, self.parse_integer)
        self.register_prefix(TokenType.FLOAT, self.parse_float)

        self.register_prefix(TokenType.STRING, self.parse_string_literal) 
        self.register_prefix(TokenType.NOT, self.parse_prefix_expression)
        self.register_prefix(TokenType.MINUS, self.parse_prefix_expression)

        self.register_prefix(TokenType.TRUE, self.parse_boolean)
        self.register_prefix(TokenType.FALSE, self.parse_boolean)

        self.register_prefix(TokenType.LPAREN, self.parse_grouped_expression)
        self.register_prefix(TokenType.LBRACKET, self.parse_array_literal)
        self.register_prefix(TokenType.LBRACE, self.parse_map_literal) 
        self.register_prefix(TokenType.ARROW, self.parse_block_statement)

        self.register_prefix(TokenType.IF, self.parse_if_expression)
        self.register_prefix(TokenType.PASS, self.parse_pass_statement)
        self.register_prefix(TokenType.WHILE, self.parse_while_statement)
        self.register_prefix(TokenType.FUNC, self.parse_function_literal)
    
        self.register_infix(TokenType.LBRACKET, self.parse_index_expression)
        self.register_infix(TokenType.LPAREN, self.parse_call_expression)
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
        self.register_infix(TokenType.PERCENT, self.parse_infix_expression)
        self.register_infix(TokenType.AND, self.parse_infix_expression)
        self.register_infix(TokenType.OR, self.parse_infix_expression)
        

    def parse_first(self, tokens):
        self.initialized = True

        self.tokens = tokens

        self.t_idx = 0
        self.current_token: Token = self.tokens[0]
        self.peek_token: Token = self.tokens[1] 

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

    def walk_back(self):
        if self.t_idx < 1:
            return
        self.t_idx -= 1 
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
            if(self.current_token.type == TokenType.EOF):
                break
            self.advance()

        return program

    def parse_statement(self):
        match self.current_token.type:
            case TokenType.VAR:
                return self.parse_var_statement()
            case TokenType.RETURN:
                return self.parse_return_statement()
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
        if(self.peek_token_is(TokenType.EQUAL)):
            statement = tree.AssignmentStatement(self.current_token, None, None)
            statement.name = tree.Identifier(self.current_token, self.current_token.literal)
            self.advance()
            self.advance()
            statement.value = self.parse_expression(PrecLevel.LOWEST)
            return statement
        return tree.Identifier(self.current_token, self.current_token.literal)

    def parse_integer(self):
        literal = tree.IntegerLiteral(self.current_token, None)
        
        try:
            value = int(self.current_token.literal)
            literal.value = value
            return literal
        except ValueError:
            ErrorHandler.error(
                self.current_token.line, f'Could not parse integer literal from {literal}')
            return None
    
    def parse_float(self):
        literal = tree.FloatLiteral(self.current_token, None)
        
        try:
            value = float(self.current_token.literal)
            literal.value = value
            return literal
        except ValueError:
            ErrorHandler.error(
                self.current_token.line, f'Could not parse float literal from {literal}')
            return None

    def parse_string_literal(self):
       return tree.StringLiteral(self.current_token, self.current_token.literal) 

    def parse_boolean(self):
        return tree.Boolean(self.current_token, self.current_token_is(TokenType.TRUE))

    def parse_grouped_expression(self):
        self.advance()

        exp = self.parse_expression(PrecLevel.LOWEST)

        if not self.expect_peek(TokenType.RPAREN):
            return None

        return exp

    def parse_array_literal(self):
        array = tree.ArrayLiteral(self.current_token)
        array.elements = self.parse_expression_list(TokenType.RBRACKET)
        return array

    def parse_map_literal(self): 
        hash = tree.MapLiteral(self.current_token, {})

        while(not self.peek_token_is(TokenType.RBRACE)):
            self.ignore_whitespace()
            self.advance()
            key = self.parse_expression(PrecLevel.LOWEST)
            

            if type(key) != tree.IntegerLiteral and type(key) != tree.StringLiteral and type(key) != tree.FloatLiteral:
                return ErrorHandler.error(self.current_token.line, "invalid type for map key")

            if not self.expect_peek(TokenType.COLON):
                return None
            
            self.ignore_whitespace()
            self.advance()
            value = self.parse_expression(PrecLevel.LOWEST)
            hash.pairs[key] = value
            self.ignore_whitespace()
            if not self.peek_token_is(TokenType.RBRACE):
                if not self.expect_peek(TokenType.COMMA):
                    return None

        if not self.expect_peek(TokenType.RBRACE):
            return None

        return hash

    def parse_expression_list(self, end):
        list = []

        self.ignore_whitespace()

        if self.peek_token_is(end):
            self.advance()
            return list
        
        self.advance()
        list.append(self.parse_expression(PrecLevel.LOWEST))

        while self.peek_token_is(TokenType.COMMA):
            self.advance()
            self.ignore_whitespace()
            self.advance()
            list.append(self.parse_expression(PrecLevel.LOWEST))
            self.ignore_whitespace()

        if not self.expect_peek(end):
            return None

        return list

    def parse_index_expression(self, left, ismap=False):
        exp = tree.IndexExpression(self.current_token, left, None)
        self.advance()
        exp.index = [self.parse_expression(PrecLevel.LOWEST)]
        if not self.expect_peek(TokenType.RBRACKET):
            return None
        while self.peek_token_is(TokenType.LBRACKET):
            self.advance()
            self.advance()
            exp.index.append(self.parse_expression(PrecLevel.LOWEST))
            if not self.expect_peek(TokenType.RBRACKET):
                return None

        if self.peek_token_is(TokenType.EQUAL):
            if isinstance(left, tree.Identifier) and not ismap:
                name = left
                self.advance()
                self.advance()
                value = self.parse_expression(PrecLevel.LOWEST)
                stmt = tree.IndexAssignment(exp.token, name, exp.index, value)
                return stmt
        return exp

    def parse_arrow_block(self):
        if(self.peek_token_is(TokenType.ARROW)):
            self.advance()
            if(self.peek_token_is(TokenType.NEWLINE)):
                return self.parse_block_statement()
            else:
                tok = self.current_token
                self.advance()
                statement = self.parse_statement()
                return tree.BlockStatement(tok, [statement])
        else:
            if(not self.expect_peek(TokenType.NEWLINE)): 
                return None
            if(not self.expect_peek(TokenType.ARROW)): 
                return None

            if(self.peek_token_is(TokenType.NEWLINE)):
                return self.parse_block_statement()
            else:
                tok = self.current_token
                self.advance()
                statement = self.parse_expression_statement()
                return tree.BlockStatement(tok, [statement])

    def parse_if_expression(self):
        expression = tree.IfExpression(self.current_token, None, None, None)

        self.advance()
        expression.condition = self.parse_expression(PrecLevel.LOWEST)
        expression.consequence = self.parse_arrow_block()
        if self.peek_token_is(TokenType.ELSE):
            self.advance()
            if(self.peek_token_is(TokenType.IF)):
                self.advance()
                expression.alternative = self.parse_if_expression()
            else:
                expression.alternative = self.parse_arrow_block()
        elif self.peek_token_is(TokenType.NEWLINE) and self.double_peek_is(TokenType.ELSE):
            self.advance()
            self.advance()
            if(self.peek_token_is(TokenType.IF)):
                self.advance()
                expression.alternative = self.parse_if_expression()
            else:
                expression.alternative = self.parse_arrow_block()
        return expression

    def parse_pass_statement(self):
        statement = tree.PassStatement(self.current_token)
        self.advance()
        return statement

    def parse_while_statement(self):
        statement = tree.WhileStatement(self.current_token, None, None)
        self.advance()
        statement.condition = self.parse_expression(PrecLevel.LOWEST)
        if not issubclass(type(statement.condition), tree.Expression):
            return ErrorHandler.error(self.current_token.line, "expected expression")
        statement.body = self.parse_arrow_block()
        return statement

    def parse_function_literal(self):
        literal = tree.FunctionLiteral(self.current_token, None, None, None, None)
        if(not self.expect_peek(TokenType.IDENTIFIER)): return None
        literal.name = tree.Identifier(self.current_token, self.current_token.literal)
        if(not self.expect_peek(TokenType.LPAREN)): return None
        literal.params = self.parse_function_parameters()

        if self.peek_token_is(TokenType.COLON):
            self.advance()
            self.advance()
            literal.return_type = self.parse_type()

        literal.body = self.parse_arrow_block()
        return literal

    def parse_function_parameters(self):
        params = []
        if(self.peek_token_is(TokenType.RPAREN)):
            self.advance()
            return params
        self.advance()
        ident = tree.Identifier(self.current_token, self.current_token.literal)
        if not self.expect_peek(TokenType.COLON): return None
        self.advance()
        t = self.parse_type()
        if t.token.type == TokenType.T_AUTO:
            return ErrorHandler.error(self.current_token.line, "auto invalid in function declaration")
        params.append((ident, t))

        while(self.peek_token_is(TokenType.COMMA)):
            self.advance()
            self.advance()
            ident = tree.Identifier(self.current_token, self.current_token.literal)
            if not self.expect_peek(TokenType.COLON): return None
            self.advance()
            t = self.parse_type()    
            if t.token.type == TokenType.T_AUTO:
                return ErrorHandler.error(self.current_token.line, "auto invalid in function declaration")
            params.append((ident, t))

        if(not self.expect_peek(TokenType.RPAREN)): return None
        return params

    def parse_call_expression(self, function):
        exp = tree.CallExpression(self.current_token, function, None)
        exp.args = self.parse_expression_list(TokenType.RPAREN)
        return exp

    def parse_call_arguments(self):
        args = []

        if(self.peek_token_is(TokenType.RPAREN)):
            self.advance()
            return args

        self.advance()
        args.append(self.parse_expression(PrecLevel.LOWEST))

        while(self.peek_token_is(TokenType.COMMA)):
            self.advance()
            self.advance()
            args.append(self.parse_expression(PrecLevel.LOWEST))

        if(not self.expect_peek(TokenType.RPAREN)):
            return None

        return args


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
            self.current_token, left, self.current_token.literal, None)


        prec = self.current_precedence()
        self.advance()
        expression.right = self.parse_expression(prec)

        return expression

    def parse_var_statement(self):
        statement = tree.VarStatement(self.current_token, None, None)

        if(not self.expect_peek(TokenType.IDENTIFIER)):
            return None

        statement.name = tree.Identifier(self.current_token, self.current_token.literal)

        if not self.expect_peek(TokenType.COLON): return None
        self.advance()

        annotation = self.parse_type()
        statement.type_annotation = annotation

        if(not self.expect_peek(TokenType.EQUAL)): return None

        self.advance()
        statement.value = self.parse_expression(PrecLevel.LOWEST)

        if(self.is_peek_end()): self.advance()

        return statement

    def parse_type(self):
        maybe = self.peek_token_is(TokenType.QUESTION)
        if not self.current_token.type in type_tokens: 
            ErrorHandler.error(self.current_token.line, "Expected valid type")

        annotation = tree.TypeAnnotation(self.current_token, maybe)
        if maybe:
            self.advance()
        return annotation

    def parse_return_statement(self):
        statement = tree.ReturnStatement(self.current_token, None)
        self.advance()
        statement.return_value = self.parse_expression(PrecLevel.LOWEST)
        if(self.is_peek_end()): self.advance()

        return statement

    def current_token_is(self, token_type):
        return self.current_token.type == token_type

    def peek_token_is(self, token_type):
        return self.peek_token.type == token_type

    def double_peek_is(self, token_type):
        n = self.t_idx+2
        if n <= len(self.tokens):
            return self.tokens[n].type == token_type
        return False

    def expect_peek(self, token_type):
        if(self.peek_token_is(token_type)):
            self.advance()
            return True
        ErrorHandler.error(
            self.peek_token.line, f'Expected {token_type}, got {self.peek_token.type}') 
        return False

    def ignore_whitespace(self):
        while(self.peek_is_whitespace()):
            self.advance()

    def peek_is_whitespace(self):
        return self.peek_token_is(TokenType.NEWLINE) or self.peek_token_is(TokenType.INDENT) or self.peek_token_is(TokenType.DEDENT)
    
    def current_is_whitespace(self):
        return self.current_token_is(TokenType.NEWLINE) or self.current_token_is(TokenType.INDENT) or self.current_token_is(TokenType.DEDENT) 

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


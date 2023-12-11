from abc import ABC, abstractmethod 
from dataclasses import dataclass

from tokentype import TokenType
from tokens import Token

class Node(ABC):
    @abstractmethod 
    def token_literal(self):
        pass

class Statement(Node):
    @abstractmethod 
    def statement_node(self):
        pass

class Expression(Node): 
    @abstractmethod 
    def expression_node(self):
        pass


class Program(Node):
    def __init__(self):
        self.statements = []

    def token_literal(self):
        if(len(self.statements) > 0):
            return self.statements[0].token_literal()
        else:
            return ""

@dataclass
class PrefixExpression(Expression):
    token: Token
    operator: str
    right: Expression

    def expression_node(self):
        pass
    def token_literal(self):
        return self.token.literal

@dataclass 
class InfixExpression(Expression):
    token: Token
    left: Expression
    operator: str
    right: Expression

    def expression_node(self):
        pass
    def token_literal(self):
        return self.token.literal

@dataclass
class Identifier(Expression):
    token: Token
    value: str

    def expression_node(self):
        pass
    def token_literal(self):
        return self.token.literal

@dataclass 
class NumLiteral(Expression):
    token: Token
    value: float

    def expression_node(self):
        pass
    def token_literal(self):
        return self.token.literal


@dataclass 
class StringLiteral(Expression):
    token: Token
    value: str

    def expression_node(self):
        pass
    def token_literal(self):
        return self.token.literal

@dataclass 
class Boolean(Expression):
    token: Token
    value: bool

    def expression_node(self):
        pass
    def token_literal(self):
        return self.token.literal

@dataclass 
class BlockStatement(Statement):
    token: Token
    statements: list[Statement]

    def statement_node(self):
        pass

    def token_literal(self):
        return self.token.literal

@dataclass 
class FunctionLiteral(Expression):
    token: Token
    name: Identifier
    params: list[Identifier]
    body: BlockStatement
    
    def expression_node(self):
        pass
    def token_literal(self):
        return self.token.literal

@dataclass
class CallExpression(Expression):
    token: Token
    function: Identifier
    args: list[Expression]

    def expression_node(self):
        pass
    def token_literal(self):
        return self.token.literal

@dataclass 
class IfExpression(Expression):
    token: Token
    condition: Expression
    consequence: BlockStatement
    alternative: BlockStatement
    
    def expression_node(self):
        pass
    def token_literal(self):
        return self.token.literal

@dataclass 
class ExpressionStatement(Statement):
    token: Token
    expression: Expression

    def statement_node(self):
        pass

    def token_literal(self):
        return self.token.literal

@dataclass
class VarStatement(Statement):
    token: Token
    name:  Identifier
    value: Expression

    def statement_node(self):
        pass

    def token_literal(self):
        return self.token.literal
       
@dataclass 
class PassStatement(Statement):
    token: Token
    
    def statement_node(self):
        pass

    def token_literal(self):
        return self.token.literal

@dataclass 
class ReturnStatement(Statement):
    token: Token
    return_value: Expression

    def statement_node(self):
        pass

    def token_literal(self):
        return self.token.literal

@dataclass 
class ModelStatement(Statement):
    token: Token
    name: Identifier

    def statement_node(self):
        pass

    def token_literal(self):
        return self.token.literal


@dataclass 
class SpawnStatement(Statement):
    token: Token
    name: Identifier

    def statement_node(self):
        pass

    def token_literal(self):
        return self.token.literal

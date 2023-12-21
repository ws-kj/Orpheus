from abc import ABC, abstractmethod 
from dataclasses import dataclass 
from tokens import Token 

class Node(ABC):
    token: Token 

    def token_literal(self):
        return self.token.literal 

class Statement(Node):
    pass 

class Expression(Node):  
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
    operator: str | None = None
    right: Expression | None = None


@dataclass
class InfixExpression(Expression):
    token: Token
    left: Expression | None = None
    operator: str | None = None
    right: Expression | None = None

@dataclass
class Identifier(Expression):
    token: Token
    value: str | None = None

@dataclass
class NumLiteral(Expression):
    token: Token
    value: float | None = None

    def __hash__(self):
        return hash((self.token, self.value))

@dataclass
class StringLiteral(Expression):
    token: Token
    value: str | None = None
    
    def __hash__(self):
        return hash((self.token, self.value))

@dataclass 
class MapLiteral(Expression):
    token: Token
    pairs: dict[Expression, Expression] 

@dataclass
class Boolean(Expression):
    token: Token
    value: bool | None = None

    def __hash__(self):
        return hash((self.token, self.value))

@dataclass
class BlockStatement(Statement):
    token: Token
    statements: list[Statement] | None = None

@dataclass
class FunctionLiteral(Expression):
    token: Token
    name: Identifier | None = None
    params: list[Identifier] | None = None
    body: BlockStatement | None = None

@dataclass
class CallExpression(Expression):
    token: Token
    function: Identifier | None = None
    args: list[Expression] | None = None

@dataclass
class IfExpression(Expression):
    token: Token
    condition: Expression | None = None 
    consequence: BlockStatement | None = None 
    alternative: BlockStatement | None = None 
  
@dataclass
class WhileStatement(Statement):
    token: Token
    condition: Expression | None = None
    body: BlockStatement | None = None

@dataclass
class ExpressionStatement(Statement):
    token: Token
    expression: Expression | None = None

@dataclass 
class ArrayLiteral(Expression):
    token: Token
    elements: list[Expression] | None = None

@dataclass
class VarStatement(Statement):
    token: Token
    name:  Identifier | None = None
    value: Expression | None = None

@dataclass
class AssignmentStatement(Statement):
    token: Token
    name:  Identifier | None = None
    value: Expression | None = None

@dataclass 
class IndexExpression(Expression):
    token: Token
    left: Expression | None = None
    index: list[Expression] | None = None 

@dataclass
class IndexAssignment(Statement):
    token: Token
    name: Identifier | None = None
    index: Expression | None = None
    value: Expression | None = None

@dataclass
class PassStatement(Statement):
    pass

@dataclass
class ReturnStatement(Statement):
    token: Token
    return_value: Expression | None = None

@dataclass
class ModelStatement(Statement):
    token: Token
    name: Identifier | None = None

@dataclass
class SpawnStatement(Statement):
    token: Token
    name: Identifier | None = None

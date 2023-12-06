import tree 
import obj
from tokentype import TokenType

FALSE = obj.Bool(False)
TRUE = obj.Bool(True)
NIL = obj.Nil()

def eval(node: tree.Node):
    match type(node):
        case tree.Program:
            return eval_statements(node.statements)
        case tree.ExpressionStatement:
            return eval(node.expression)
        case tree.PrefixExpression:
            right = eval(node.right)
            return eval_prefix_expression(node, right)
        case tree.NumLiteral:
            return obj.Number(node.value)
        case tree.Boolean:
            return native_bool(node.value)
        case _:
            return None

def eval_statements(statements):
    result = None
    for statement in statements:
        result = eval(statement)

    return result

def eval_prefix_expression(node, right):
    match node.token.type:
        case TokenType.NOT:
            return native_bool(eval_not_expression(right))
        case TokenType.MINUS:
            return eval_minus_prefix_expression(right)
        case _:
            return None

def eval_not_expression(right):
    return not right.value # will eventually rewrite in C

def eval_minus_prefix_expression(right):
    print(right)
    if(right.type() != obj.ObjectType.NUMBER):
        return None
    value = right.value
    return obj.Number(-value)

def native_bool(value):
    if(value): return TRUE
    return FALSE

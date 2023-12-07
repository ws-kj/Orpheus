import tree 
import obj
from tokentype import TokenType

FALSE = obj.Bool(False)
TRUE = obj.Bool(True)
NIL = obj.Nil()

def eval(node: tree.Node):
    match type(node):
        case tree.Program:
            return eval_program(node.statements)
        case tree.ExpressionStatement:
            return eval(node.expression)
        case tree.PrefixExpression:
            right = eval(node.right)
            return eval_prefix_expression(node, right)
        case tree.InfixExpression:
            left = eval(node.left)
            right = eval(node.right)
            return eval_infix_expression(node, left, right)
        case tree.BlockStatement:
            return eval_block_statement(node.statements)
        case tree.ReturnStatement:
            val = eval(node.return_value)
            return obj.ReturnValue(val)
        case tree.IfExpression:
            return eval_if_expression(node)
        case tree.NumLiteral:
            return obj.Number(node.value)
        case tree.Boolean:
            return bool_obj(node.value)
        case _:
            return None

def eval_program(statements):
    result = None
    for statement in statements:
        result = eval(statement)

        if type(result) == obj.ReturnValue:
            return result

    return result

def eval_block_statement(statements):
    result = None
    for statement in statements:
        result = eval(statement)

        if result != None and result.type() == obj.ObjectType.RETURN_VALUE:
            return result

    return result

def eval_prefix_expression(node, right):
    match node.token.type:
        case TokenType.NOT:
            return bool_obj(eval_not_expression(right))
        case TokenType.MINUS:
            return eval_minus_prefix_expression(right)
        case _:
            return None

def eval_infix_expression(node, left, right):
    left_val = left.value
    right_val = right.value

    match node.token.type:
        case TokenType.PLUS:
            return obj.Number(left_val + right_val)
        case TokenType.MINUS:
            return obj.Number(left_val - right_val)
        case TokenType.STAR:
            return obj.Number(left_val * right_val)
        case TokenType.SLASH:
            return obj.Number(left_val / right_val)
        case TokenType.LESS:
            return bool_obj(left_val < right_val)
        case TokenType.GREATER:
            return bool_obj(left_val > right_val)
        case TokenType.LESS_EQUAL:
            return bool_obj(left_val <= right_val)
        case TokenType.GREATER_EQUAL:
            return bool_obj(left_val >= right_val)
        case TokenType.EQUAL_EQUAL:
            return bool_obj(left_val == right_val)
        case TokenType.BANG_EQUAL:
            return bool_obj(left_val != right_val)
        case _:
            return None

def eval_not_expression(right):
    return not right.value # will eventually rewrite in C

def eval_minus_prefix_expression(right):
    if(right.type() != obj.ObjectType.NUMBER):
        return None
    return obj.Number(-right.value)

def eval_if_expression(node):
    cond = eval(node.condition)
    if(is_truthy(cond)):
        return eval(node.consequence)
    elif(node.alternative != None):
        return eval(node.alternative)
    else:
        return None

def is_truthy(obj):
    if(obj == NIL or obj == FALSE):
        return False
    return True

def bool_obj(value):
    if(value): return TRUE
    return FALSE

import tree 
import obj
from tokentype import TokenType
from environment import Environment
from error_handler import ErrorHandler
from builtin import builtins

FALSE = obj.Bool(False)
TRUE = obj.Bool(True)
NIL = obj.Nil()


def eval(node, env):
    match type(node):
        case tree.Program:
            return eval_program(node.statements, env)
        case tree.ExpressionStatement:
            return eval(node.expression, env)
        case tree.PrefixExpression:
            right = eval(node.right, env)
            if(is_error(right)): return right
            return eval_prefix_expression(node, right, env)
        case tree.InfixExpression:
            left = eval(node.left, env)
            right = eval(node.right, env)
            if(is_error(left)): return left
            if(is_error(right)): return right
            return eval_infix_expression(node, left, right, env)
        case tree.BlockStatement:
            return eval_block_statement(node.statements, env)
        case tree.ReturnStatement:
            val = eval(node.return_value, env)
            if(is_error(val)): return val
            return obj.ReturnValue(val)
        case tree.IfExpression:
            return eval_if_expression(node, env)
        case tree.WhileStatement:
            eval_while_statement(node, env)
        case tree.VarStatement:
            val = eval(node.value, env)
            if(is_error(val)): return val
            env.set(node.name.value, val)
        case tree.AssignmentStatement:
            eval_assignment_statement(node, env)
        case tree.NumLiteral:
            return obj.Number(node.value)
        case tree.StringLiteral:
            return obj.String(node.value)
        case tree.Boolean:
            return bool_obj(node.value)
        case tree.Identifier:
            return eval_ident(node, env)
        case tree.FunctionLiteral:
            func_obj = obj.Function(node.name, node.params, node.body, env)
            env.set(node.name.value, func_obj)
            return func_obj
        case tree.CallExpression:
            func = eval(node.function, env)
            if(is_error(func)): 
                return func
            args = eval_expressions(node.args, env)
            if(len(args) == 1 and is_error(args[0])): 
                return args[0]
            return apply_function(func, args) 
        case tree.ArrayLiteral:
            elements = eval_expressions(node.elements, env)
            if len(elements) == 1 and is_error(elements[0]):
                return elements[0]
            return obj.Array(elements)
        case tree.IndexExpression:
            left = eval(node.left, env)
            if is_error(left): return left
            index = eval(node.index, env)
            if is_error(index): return index
            return eval_index_expression(left, index)
        case tree.IndexAssignment:
            return eval_index_assignment(node, env)
        case _:
            return None

def eval_program(statements, env):
    result = None
    for statement in statements:
        result = eval(statement, env)

        match type(result):
            case obj.Error:
                return result
            case _:
                pass
    return result

def eval_expressions(exps, env):
    result = []
    for e in exps:
        evaluated = eval(e, env)
        if(is_error(evaluated)): return [evaluated]
        result.append(evaluated)
    return result

def apply_function(func, args):
    match type(func):
        case obj.Function:
            ext_env = extend_func_env(func, args)
            evaluated = eval(func.body, ext_env)
            return unwrap_return_value(evaluated)
        case obj.Builtin:
            return func.function(args)
        case _:
            return ErrorHandler.runtime_error(f'not a function: {type(func)}')

def extend_func_env(func, args):
    env = Environment(outer=func.env)
    for idx, param in enumerate(func.params):
        env.set(param.value, args[idx])

    return env

def unwrap_return_value(return_obj):
    if type(return_obj) == obj.ReturnValue:
        return return_obj.value

    return return_obj

def eval_block_statement(statements, env):
    result = None
    for statement in statements:
        result = eval(statement, env)

        if result != None:
            match result.type():
                case obj.ObjectType.RETURN_VALUE:
                    return result
                case obj.ObjectType.ERROR:
                    return result
                case _:
                    pass
    return result

def eval_ident(node, env):
    val = env.get(node.value)
    if val != None:
        return val
    func = builtins.get(node.value)
    if func != None:
        return func
    return ErrorHandler.runtime_error(f'unknown identifier: {node.value}')

def eval_assignment_statement(node, env):
    current = env.get(node.name.value)
    if current != None:
        val = eval(node.value, env)
        if is_error(val): return val
        env.set(node.name.value, val)
        return
    return ErrorHandler.runtime_error(f'unknown variable: {node.name.value}')

def eval_index_assignment(node, env):
    current = env.get(node.name.value)
    if current != None:
        idx_obj = eval(node.index, env)
        if is_error(idx_obj): return idx_obj
        val = eval(node.value, env)
        if is_error(val): return val

        idx = int(idx_obj.value)
        elements = current.elements
        if idx < 0 or idx > len(elements)-1:
            return ErrorHandler.runtime_error(f'index `{idx}` out of range')
        elements[idx] = val
        env.set(node.name.value, obj.Array(elements))
        return
    return ErrorHandler.runtime_error(f'unknown variable: {node.name.value}')

def eval_prefix_expression(node, right, env):
    match node.token.type:
        case TokenType.NOT:
            return bool_obj(eval_not_expression(right, env))
        case TokenType.MINUS:
            return eval_minus_prefix_expression(right, env)
        case _:
            return ErrorHandler.runtime_error(f'unknown operator:  {str(node.token.literal)} {right.type()}')

def eval_index_expression(left, index):
    if left.type() == obj.ObjectType.ARRAY and index.type() == obj.ObjectType.NUMBER:
        return eval_array_index_expression(left, index)
    else:
        return ErrorHandler.runtime_error(f'index operator not supported: {left.token.literal}')

def eval_array_index_expression(array, index):
    idx = int(index.value)
    max = len(array.elements) - 1
    if idx < 0 or idx > max: 
        return ErrorHandler.runtime_error(f'index `{idx}` out of range')

    return array.elements[idx]

def eval_infix_expression(node, left, right, env):
    left_val = left.value
    right_val = right.value
    if(left.type() != right.type()):
        return ErrorHandler.runtime_error(f'type mismatch: {left.type()} {right.type()}')

    match left.type():
        case obj.ObjectType.NUMBER:
            match node.token.type:
                case TokenType.PLUS:
                    return obj.Number(left_val + right_val)
                case TokenType.MINUS:
                    return obj.Number(left_val - right_val)
                case TokenType.STAR:
                    return obj.Number(left_val * right_val)
                case TokenType.SLASH:
                    return obj.Number(left_val / right_val)
                case TokenType.PERCENT:
                    return obj.Number(left_val % right_val)
                case TokenType.LESS:
                    return bool_obj(left_val < right_val)
                case TokenType.GREATER:
                    return bool_obj(left_val > right_val)
                case TokenType.LESS_EQUAL:
                    return bool_obj(left_val <= right_val)
                case TokenType.GREATER_EQUAL:
                    return bool_obj(left_val >= right_val)
                case _:
                    pass
        case obj.ObjectType.STRING:
            match node.token.type:
                case TokenType.PLUS:
                    return obj.String(left_val + right_val)
                case _:
                    pass
        case obj.ObjectType.BOOL:
            match node.token.type:
                case TokenType.AND:
                    return obj.Bool(left_val and right_val)
                case TokenType.OR:
                    return obj.Bool(left_val or right_val)

    match node.token.type:
        case TokenType.EQUAL_EQUAL:
            return bool_obj(left_val == right_val)
        case TokenType.BANG_EQUAL:
            return bool_obj(left_val != right_val)
        case _:
            return ErrorHandler.runtime_error(f'unknown operator: {left.type()} {node.token.literal} {right.type()}')

def eval_not_expression(right, env):
    return not right.value # will eventually rewrite in C

def eval_minus_prefix_expression(right, env):
    if(right.type() != obj.ObjectType.NUMBER):
        return ErrorHandler.runtime_error(f'unknown operator: {str(right.type())}')
    return obj.Number(-right.value)

def eval_if_expression(node, env):
    cond = eval(node.condition, env)
    if(is_error(cond)): return cond
    if(is_truthy(cond)):
        return eval(node.consequence, env)
    elif(node.alternative != None):
        return eval(node.alternative, env)
    else:
        return None

def eval_while_statement(node, env):
    cond = eval(node.condition, env)
    if(is_error(cond)): return cond
    if cond.type() != obj.ObjectType.BOOL:
        return ErrorHandler.runtime_error(f'expected boolean for while condition')
    while(cond.value == True):
        eval(node.body, env)
        cond = eval(node.condition, env)
        if(is_error(cond)): return cond

def is_truthy(obj):
    if(obj == NIL or obj == FALSE):
        return False
    return True

def is_error(o):
    if o != None:
        return o.type() == obj.ObjectType.ERROR
    return False

def bool_obj(value):
    if(value): return TRUE
    return FALSE

def error(msg):
    return obj.Error(message=msg)

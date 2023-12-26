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
            res = eval_var_statement(node, env)
            if(is_error(res)): return res
        case tree.AssignmentStatement:
            res = eval_assignment_statement(node, env)
            if is_error(res): return res
        case tree.IntegerLiteral:
            return obj.Integer(node.value)
        case tree.FloatLiteral:
            return obj.Float(node.value)
        case tree.StringLiteral:
            return obj.String(node.value)
        case tree.Boolean:
            return bool_obj(node.value)
        case tree.Identifier:
            return eval_ident(node, env)
        case tree.Nil:
            return obj.Nil()
        case tree.TypeAnnotation:
            return obj.TypeSig(obj.tok_objs[node.token.type]) 
            # for type(var) == int, we dont want to cmp maybe/any since val is all we care about
        case tree.FunctionLiteral:
            return eval_function_literal(node, env)
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
            indices = []
            for i in node.index:
                index = eval(i, env)
                if is_error(index): return index
                indices.append(index)
            return eval_index_expression(left, indices)
        case tree.IndexAssignment:
            return eval_index_assignment(node, env)
        case tree.MapLiteral:
            return eval_map_literal(node, env)
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

def eval_var_statement(node, env):
    val = eval(node.value, env)
    if(is_error(val)): return val

    tsig = obj.typesig(node.type_annotation)
    if(tsig == None): return ErrorHandler.runtime_error('bad type signature')

    if tsig.obj_type == None and not tsig.any:
        tsig.obj_type = val.type()

    return try_assign(node.name.value, val, env, tsig)

def try_assign(name, val, env, tsig=None):
    if tsig == None:
        tsig = env.get_tsig(name)
        if tsig == None:
            return None
   
    if tsig.obj_type != obj.ObjectType.ANY and val.type() != tsig.obj_type:
        if not (tsig.maybe and val.type() == obj.ObjectType.NIL):
            return ErrorHandler.runtime_error(f'incompatible types: {val.type()} {tsig.obj_type}')

    env.set(name, val, tsig)

def eval_function_literal(node, env):
    tsig = None
    if node.return_type != None:
        tsig = obj.typesig(node.return_type)
        print(node.return_type)
        if(tsig == None): return ErrorHandler.runtime_error('bad return type')

    func_obj = obj.Function(node.name, node.params, node.body, env, tsig)
    try_assign(node.name.value, func_obj, env, obj.TypeSig(obj.ObjectType.FUNCTION))
    #return func_obj

def apply_function(func, args):
    match type(func):
        case obj.Function:
            ext_env = extend_func_env(func, args)
            if is_error(ext_env): return ext_env
            evaluated = eval(func.body, ext_env)
            
            if evaluated == None:
                ret = obj.Nil()
            else:
                ret = unwrap_return_value(evaluated, func.tsig)
                if ret == None:
                    ret = obj.Nil()
            return ret
        case obj.Builtin:
            return func.function(args)
        case _:
            return ErrorHandler.runtime_error(f'not a function: {type(func)}')

def extend_func_env(func, args):
    env = Environment(outer=func.env)
    for idx, param in enumerate(func.params):
        name = param[0].value
        val  = args[idx]
        tsig = obj.typesig(param[1])
        res = try_assign(name, val, env, tsig)
        if is_error(res): return res

    return env

def unwrap_return_value(return_obj, tsig=None):
    if type(return_obj) == obj.ReturnValue: 
        return_obj = return_obj.value

    if tsig != None and not tsig.any:
        if return_obj.type() != tsig.obj_type:
            if not (return_obj.type() == obj.ObjectType.NIL and tsig.maybe):
                return ErrorHandler.runtime_error(f'incompatible return type. expected {tsig.obj_type} got {return_obj.type()}')
    return return_obj

def eval_block_statement(statements, env):
    result = None
    new = Environment(outer=env)
    for statement in statements:
        result = eval(statement, new)

        if result != None:
            match result.type():
                case obj.ObjectType.RETURN_VALUE:
                    break
                case obj.ObjectType.ERROR:
                    break
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
        if is_error(val): 
            return val
        return try_assign(node.name.value, val, env)
    return ErrorHandler.runtime_error(f'unknown variable: {node.name.value}')

def eval_index_assignment(node, env):
    current = env.get(node.name.value)
    old = current
    if current == None:
        return ErrorHandler.runtime_error(f'unknown variable: {node.name.value}')

    val = eval(node.value, env)
    if is_error(val): return val
    
    idx_vals = []
    c = 0
    final = False
    for index in node.index:
        if c == len(node.index)-1:
            final = True
        idx_obj = eval(index, env)
        if is_error(idx_obj): return idx_obj
        match current.type():
            case obj.ObjectType.ARRAY:
                if idx_obj.type() != obj.ObjectType.INTEGER:
                    return ErrorHandler.runtime_error(f'invalid index type {idx_obj.type()}')
                if idx_obj.value % 1 != 0:
                    return ErrorHandler.runtime_error(f'index must be integer')
                idx = int(idx_obj.value)
                elements = current.elements
                if idx < 0 or idx > len(elements)-1:
                    return ErrorHandler.runtime_error(f'index `{idx}` out of range')
                idx_vals.append(idx)
                if final:
                    current.elements[idx] = val
                else:
                    current = current.elements[idx]
            case obj.ObjectType.MAP:
                key = idx_obj
                hashed = obj.hash_obj(key)
                idx_vals.append((hashed, key))
                if final:
                    current.pairs[hashed] = obj.MapPair(key, val)
                else:
                    current = current.pairs[hashed].value
            case _:
                return ErrorHandler.runtime_error(f'invalid index on {current.type()}')
        c += 1

def eval_index_expression(left, indices):
    ref = left
    for index in indices:
        match ref.type():
            case obj.ObjectType.ARRAY:
                if index.type() != obj.ObjectType.INTEGER:
                    return ErrorHandler.runtime_error(f'invalid index type {index.type()}')
                if index.value % 1 != 0:
                    return ErrorHandler.runtime_error(f'index must be integer')
                idx = int(index.value) 
                max = len(ref.elements) - 1
                if idx < 0 or idx > max: 
                    return ErrorHandler.runtime_error(f'index `{idx}` out of range')
                ref = ref.elements[idx]
            case obj.ObjectType.MAP:
                if not index.type() in obj.hashable:
                    return ErrorHandler.runtime_error(f'unusable as hash key: {index.type()}')

                key = obj.hash_obj(index)
                if key not in ref.pairs.keys():
                    return ErrorHandler.runtime_error(f'no value for key: {index.inspect()}')

                ref = ref.pairs[key].value
            case _:
                return ErrorHandler.runtime_error(f'invalid index on {ref.type()}')
    return ref


def eval_prefix_expression(node, right, env):
    match node.token.type:
        case TokenType.NOT:
            return bool_obj(eval_not_expression(right, env))
        case TokenType.MINUS:
            return eval_minus_prefix_expression(right, env)
        case _:
            return ErrorHandler.runtime_error(f'unknown operator:  {str(node.token.literal)} {right.type()}')

def eval_map_literal(node, env):
    pairs = {}
    for key_node, value_node in node.pairs.items():
        key = eval(key_node, env)
        if is_error(key): return key

        value = eval(value_node, env)
        if is_error(value): return value

        hashed = obj.hash_obj(key)
        pairs[hashed] = obj.MapPair(key, value)

    return obj.Map(pairs)

def eval_infix_expression(node, left, right, env):

    if node.token.type == TokenType.IS:
        if right.type() != obj.ObjectType.TYPE:
            return ErrorHandler.runtime_error(f'expected valid type, got {right.type()}')
        return bool_obj(left.type() == right.obj_type)

    if(left.type() != right.type()):
        return ErrorHandler.runtime_error(f'type mismatch: {left.type()} {right.type()}')

    if not hasattr(left, "value"):
        return ErrorHandler.runtime_error(f'invalid operator on type {left.type()}')
    if not hasattr(right, "value"):
        return ErrorHandler.runtime_error(f'invalid operator on type {right.type()}')

    left_val = left.value
    right_val = right.value

    match left.type():
        case obj.ObjectType.INTEGER:
            match node.token.type:
                case TokenType.PLUS:
                    return obj.Integer(left_val + right_val)
                case TokenType.MINUS:
                    return obj.Integer(left_val - right_val)
                case TokenType.STAR:
                    return obj.Integer(left_val * right_val)
                case TokenType.SLASH:
                    return obj.Integer(left_val / right_val)
                case TokenType.PERCENT:
                    return obj.Integer(left_val % right_val)
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
    if(right.type() != obj.ObjectType.INTEGER):
        return ErrorHandler.runtime_error(f'unknown operator: {str(right.type())}')
    return obj.Integer(-right.value)

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
    if o != None and hasattr(o, 'type') and callable(o.type):
        return o.type() == obj.ObjectType.ERROR
    return False

def bool_obj(value):
    if(value): return TRUE
    return FALSE

def error(msg):
    return obj.Error(message=msg)

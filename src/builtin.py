import obj
from error_handler import ErrorHandler

def __len(args):
    if(len(args) != 1):
        return ErrorHandler.runtime_error(f'len: expected 1 argument, got {len(args)}')

    match type(args[0]):
        case obj.String:
            return obj.Integer(int(len(args[0].value)))
        case obj.Array:
            return obj.Integer(int(len(args[0].elements)))
        case obj.Map:
            return obj.Integer(int(len(args[0].pairs)))
        case _:
            return ErrorHandler.runtime_error(f'len: arg type {args[0].type()} not supported')

def __print(args):
    if(len(args) > 1):
        return ErrorHandler.runtime_error(f'print: expected 1 argument, got {len(args)}')
    if(len(args) == 0): 
        return obj.Nil()
    if hasattr(args[0], 'value'):
        raw = str(args[0].value).encode().decode("unicode_escape")
        print(raw, end='')
    elif hasattr(args[0], 'inspect'):
        raw = str(args[0].inspect()).encode().decode("unicode_escape")
        print(raw, end='')
    else:
        return ErrorHandler.runtime_error(f'print: cannot print type {args[0].type()}')

    return obj.Nil()

def __println(args):
    if(len(args) > 1):
        return ErrorHandler.runtime_error(f'println: expected 1 argument, got {len(args)}')

    if(len(args) == 0):
        print()
        return obj.Nil()
    if hasattr(args[0], 'value'):
        raw = str(args[0].value).encode().decode("unicode_escape")
        print(raw)
    elif hasattr(args[0], 'inspect'):
        raw = str(args[0].inspect()).encode().decode("unicode_escape")
        print(raw)
    else:
        return ErrorHandler.runtime_error(f'println: cannot print type {args[0].type()}')

    return obj.Nil()

def __tail(args):
    if len(args) != 1:
        return ErrorHandler.runtime_error(f'tail: expected 1 argument, got {len(args)}')

    if args[0].type() != obj.ObjectType.ARRAY:
        return ErrorHandler.runtime_error(f'tail: argument must be array')

    array = args[0]
    if len(array.elements) > 1:
        new = array.elements[1:]
        return obj.Array(new)
    return obj.Nil()

def __append(args):
    if len(args) != 2:
        return ErrorHandler.runtime_error(f'append: expected 2 arguments, got {len(args)}')
    if args[0].type() != obj.ObjectType.ARRAY:
        return ErrorHandler.runtime_error(f'append: first argument must be array')
    array = args[0].elements
    array.append(args[1])
    return obj.Array(array)

def __type(args):
    if len(args) != 1:
        return ErrorHandler.runtime_error(f'type: expected 1 argument, got {len(args)}')
    return obj.TypeSig(args[0].type())

def __has(args):
    if len(args) != 1:
        return ErrorHandler.runtime_error(f'has: expected 1 argument, got {len(args)}')

    return obj.Bool(args[0].type() != obj.ObjectType.NIL)

builtins = {
    "len": obj.Builtin(__len),
    "print": obj.Builtin(__print),
    "println": obj.Builtin(__println),
    "tail": obj.Builtin(__tail),
    "append": obj.Builtin(__append),
    "type": obj.Builtin(__type),
    "has": obj.Builtin(__has)
}


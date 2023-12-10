import obj
from error_handler import ErrorHandler

def bf_len(args):
    if(len(args) != 1):
        return ErrorHandler.runtime_error(f'len: expected 1 argument, got {len(args)}')

    match type(args[0]):
        case obj.String:
            return obj.Number(float(len(args[0].value)))
        case _:
            return ErrorHandler.runtime_error(f'len: arg type {args[0].type()} not supported')

def bf_print(args):
    if(len(args) != 1):
        return ErrorHandler.runtime_error(f'print: expected 1 argument, got {len(args)}')
    
    match type(args[0]):
        case obj.String:
            raw = args[0].value.encode().decode("unicode_escape")
            print(raw, end="")
        case _:
            return ErrorHandler.runtime_error(
                f'print: arg type {args[0].type()} not supported')

    return obj.Nil()

def bf_println(args):
    if(len(args) != 1):
        return ErrorHandler.runtime_error(f'print: expected 1 argument, got {len(args)}')
    
    match type(args[0]):
        case obj.String:
            raw = args[0].value.encode().decode("unicode_escape")
            print(raw)
        case _:
            return ErrorHandler.runtime_error(
                f'print: arg type {args[0].type()} not supported')

    return obj.Nil()

builtins = {
    "len": obj.Builtin(bf_len),
    "print": obj.Builtin(bf_print),
    "println": obj.Builtin(bf_println)
}


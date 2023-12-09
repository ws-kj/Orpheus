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

builtins = {
    "len": obj.Builtin(bf_len)
}


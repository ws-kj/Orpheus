import obj

class ErrorHandler(object):
    had_error = False

    @classmethod
    def error(cls, line: int, msg: str):
        cls.report(line, "", msg)

    @classmethod 
    def runtime_error(cls, msg: str):
        cls.error(-1, msg)
        return obj.Error(message = msg)

    @classmethod
    def report(cls, line: int, where: str, msg: str):
        if(line != -1): print("[" + str(line) + "] error " + where + ": " + msg)
        cls.had_error = True



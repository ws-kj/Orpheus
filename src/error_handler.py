class ErrorHandler(object):
    had_error = False

    @classmethod
    def error(cls, line: int, msg: str):
        cls.report(line, "", msg)

    @classmethod
    def report(cls, line: int, where: str, msg: str):
        print("[" + str(line) + "] error " + where + ": " + msg)
        cls.had_error = True


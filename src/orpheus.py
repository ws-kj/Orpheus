import sys

from tokentype import TokenType
from tokens import Token
from scanner import Scanner
from parser import Parser
from error_handler import ErrorHandler
from eval import eval

class Orpheus(object):
    had_error = False

    def run_file(self, fname: str):
        with open(fname) as file:
            source = file.read()
        
        self.run(source)

        if(ErrorHandler.had_error):
            sys.exit(1)

    # TODO Fix
    def run_repl(self):
        parser = Parser()
        while True:
            line = input("> ")
            if line.strip() == "":
                break

            self.run(line, parser)
            ErrorHandler.had_error = False

    def run(self, source: str, parser=None):
        tokens = Scanner(source).scan_tokens()
        for tok in tokens:
            #print(tok)
            pass

        if(parser==None):
            parser = Parser(tokens)
        else:
            parser.tokens += tokens

        program = parser.parse_program()

        for s in program.statements:
            print(s)
            print()

        print("Eval:")
        print(eval(program).inspect())

if __name__ == "__main__":
    interpreter = Orpheus()

    if(len(sys.argv) == 2):
        interpreter.run_file(sys.argv[1])
    elif(len(sys.argv) > 2):
        print("Usage: orpheus [file]")
        sys.exit(0)
    else:
        interpreter.run_repl()
        

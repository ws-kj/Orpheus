from dataclasses import dataclass

from tokentype import TokenType

@dataclass 
class Token:
    type: TokenType
    literal: str 
    line: int
    
    def __str__(self):
        res = f'{self.type} {self.lexeme}'
        if self.literal != None:
            res += f' {self.literal}'

        return res

    def get_literal(self):
        return self.literal


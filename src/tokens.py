from dataclasses import dataclass

from tokentype import TokenType

@dataclass 
class Token:
    type: TokenType
    literal: str 
    line: int
    
    def __str__(self):
        res = f'{self.type}'
        if self.literal != None:
            res += f' {self.literal}'

        return res

    def __hash__(self):
        return hash((self.type, self.literal, self.line))

    def get_literal(self):
        return self.literal


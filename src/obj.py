from abc import ABC, abstractmethod 
from dataclasses import dataclass
from enum import Enum, auto

from environment import Environment
import tree 

class ObjectType(Enum):
    NUMBER = auto()
    STRING = auto()
    BOOL = auto()
    NIL = auto()
    RETURN_VALUE = auto()
    ERROR = auto()
    FUNCTION = auto()

class Object(ABC):
    @abstractmethod 
    def type(self):
        pass
    @abstractmethod 
    def inspect(self):
        pass

@dataclass
class Number(Object):
    value: float

    def inspect(self):
        return str(self.value)
    def type(self):
        return ObjectType.NUMBER

@dataclass
class String(Object):
    value: str

    def inspect(self):
        return "\"" + self.value + "\""
    def type(self):
        return ObjectType.STRING

@dataclass
class Bool(Object):
    value: bool

    def inspect(self):
        return str(self.value)
    def type(self):
        return ObjectType.BOOL

@dataclass
class Nil(Object):
    value = None
    def inspect(self):
        return "nil"
    def type(self):
        return ObjectType.NIL

@dataclass
class ReturnValue(Object):
    value: Object
    def inspect(self):
        return str(self.value.inspect())
    def type(self):
        return ObjectType.RETURN_VALUE

@dataclass 
class Error(Object):
    message: str

    def inspect(self):
        return "ERROR: " + self.message
    def type(self):
        return ObjectType.ERROR

@dataclass 
class Function(Object):
    name: tree.Identifier
    params: list[tree.Identifier]
    body: tree.BlockStatement
    env: Environment

    def inspect(self):
        return f'func {self.name.value}'
    def type(self):
        return ObjectType.FUNCTION

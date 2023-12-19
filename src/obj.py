from abc import ABC, abstractmethod 
from dataclasses import dataclass
from typing import Callable
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
    BUILTIN = auto()
    ARRAY = auto()
    HASH = auto()

class Object(ABC):
    pass

@dataclass
class HashKey:
    type: ObjectType
    value: int
    
    def __hash__(self):
        return hash((self.type, self.value))

def hash_obj(o):
    return HashKey(o.type(), hash(o))

@dataclass 
class MapPair:
    key: Object
    value: Object


@dataclass
class Map(Object):
    pairs: dict[HashKey, MapPair]

    def inspect(self):
        ret = "{"
        l = len(self.pairs.values())
        i = 0
        for pair in self.pairs.values():
            ret += pair.key.inspect()
            ret += ": "
            ret += pair.value.inspect()            
            if i < l-1: ret += ", "
            i += 1
        ret += "}"
        return ret

    def type(self):
        return ObjectType.HASH

@dataclass
class Number(Object):
    value: float

    def inspect(self):
        return str(self.value)
    def type(self):
        return ObjectType.NUMBER
    
    def __hash__(self):
        return hash(self.value)


@dataclass
class String(Object):
    value: str

    def inspect(self):
        return "\"" + self.value + "\""
    def type(self):
        return ObjectType.STRING
    
    def __hash__(self):
        return hash(self.value)


@dataclass
class Bool(Object):
    value: bool

    def inspect(self):
        if self.value:
            return "true"
        return "false"

    def type(self):
        return ObjectType.BOOL
    
    def __hash__(self):
        return hash(self.value)

@dataclass
class Nil(Object):
    value = None
    def inspect(self):
        return "nil"
    def type(self):
        return ObjectType.NIL
    
    def __hash__(self):
        return hash(self.value)

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

BuiltinFunction = Callable[..., Object]
@dataclass 
class Builtin(Object):
    function: BuiltinFunction

    def inspect(self):
        return "builtin function"
    def type(self):
        return ObjectType.BUILTIN

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

@dataclass
class Array(Object):
    elements: list[Object]

    def inspect(self):
        ret = "["
        for i in range(0, len(self.elements)):
            ret += self.elements[i].inspect()
            if i != len(self.elements)-1:
                ret += ", "
        ret += "]"
        return ret
    def type(self):
        return ObjectType.ARRAY

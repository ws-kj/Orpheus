from abc import ABC, abstractmethod 
from dataclasses import dataclass
from enum import Enum, auto

class ObjectType(Enum):
    NUMBER = auto()
    BOOL = auto()
    NIL = auto()
    RETURN_VALUE = auto()
    ERROR = auto()

class Object(ABC):
    value: object
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
    obj: Object
    def inspect(self):
        return str(self.obj.inspect())
    def type(self):
        return ObjectType.RETURN_VALUE

@dataclass 
class Error(Object):
    message: str

    def inspect(self):
        return "ERROR: " + self.message
    def type(self):
        return ObjectType.ERROR

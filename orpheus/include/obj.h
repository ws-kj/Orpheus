#pragma once

#include <ostream>
#include <unordered_map>

#include "tokentype.h"

enum class ObjectType {
    INTEGER,
    FLOAT,
    STRING,
    BOOL,
    NIL,
    RETURN_VALUE,
    ERROR,
    FUNCTION,
    BUILTIN,
    LIST,
    MAP,
    ANY,
    TYPE
};

class Object {
    ObjectType type;

public:
    Object(ObjectType type) : type(type) {}
    ObjectType GetType() const { return type; };
};

class Type : public Object {
public:
    Type(ObjectType type, bool maybe, bool any)
        : Object(type), value(type), maybe(maybe), any(any) {}

    ObjectType value;
    bool maybe;
    bool any;
};

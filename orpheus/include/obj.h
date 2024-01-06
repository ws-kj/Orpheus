#pragma once

#include <ostream>
#include <unordered_map>
#include <memory>

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
    ObjectType GetType() const  { return type; };
    virtual size_t Hash() const { return 0; };
};

class HashKey {
public: 
    ObjectType type;
    size_t value;

    HashKey(ObjectType type, size_t value) : type(type), value(value) {}

    std::size_t operator()() const { return static_cast<std::size_t>(type) ^ value; }
};
HashKey HashObj(std::shared_ptr<Object> object);

struct MapPair {
    std::shared_ptr<Object> key;
    std::shared_ptr<Object> value;
};

class Type : public Object {
public:
    Type(ObjectType type, bool maybe, bool any)
        : Object(type), value(type), maybe(maybe), any(any) {}

    ObjectType value;
    bool maybe;
    bool any;
};

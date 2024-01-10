#pragma once

#include <ostream>
#include <unordered_map>
#include <memory>
#include <functional>

#include "tokentype.h"
#include "ast.h"

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
    virtual std::string Inspect() const { return "object"; }; 
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

struct Entry {
    std::shared_ptr<Object> value;
    std::shared_ptr<Type> type;
};

class Environment {
    std::map<std::string, Entry> store;
    std::shared_ptr<Environment> outer;

public:
    Environment(std::shared_ptr<Environment> outer = nullptr)
        : outer(outer), store({}) {}

    std::shared_ptr<Object> Get(const std::string& name);
    std::shared_ptr<Type>  GetType(const std::string& name);
    std::shared_ptr<Object> Set(const std::string& name, std::shared_ptr<Object> obj, std::shared_ptr<Type> type=nullptr);
};


class Float : public Object {
public:
    float value;
    Float(float value) : Object(ObjectType::FLOAT), value(value) {}
};

class Integer : public Object {
public:
    int value;
    Integer(int value) : Object(ObjectType::INTEGER), value(value) {}
};

class String : public Object {
public:
    std::string value;
    String(const std::string& value) : Object(ObjectType::STRING), value(value) {}
};

class Bool : public Object {
public:
    bool value;
    Bool(bool value) : Object(ObjectType::BOOL), value(value) {}
};

class Nil : public Object {
public:
    Nil() : Object(ObjectType::NIL) {}
};

class ReturnValue : public Object {
public:
    std::shared_ptr<Object> object;
    ReturnValue(std::shared_ptr<Object> object) : Object(ObjectType::RETURN_VALUE), object(object) {}
};

class Error : public Object {
public:
    std::string message;
    Error(const std::string& message) : Object(ObjectType::ERROR), message(message) {}
};

class Builtin : public Object {
public:
    using BuiltinFunction = void(*)(const std::vector<std::shared_ptr<Object>>&);
    BuiltinFunction function;
    Builtin(BuiltinFunction function) : Object(ObjectType::BUILTIN), function(function) {}
};

class Function : public Object {
public:
    std::shared_ptr<Identifier> name;
    std::vector<std::shared_ptr<Identifier>> params;
    std::shared_ptr<BlockExpression> body;
    std::shared_ptr<Environment> env;
    std::shared_ptr<Type> type;

    Function(
        std::shared_ptr<Identifier> name,
        const std::vector<std::shared_ptr<Identifier>>& params,
        std::shared_ptr<BlockExpression> body,
        std::shared_ptr<Environment> env,
        std::shared_ptr<Type> type
    ) : Object(ObjectType::FUNCTION), name(name), params(params), body(body), env(env), type(type) {}
};  

class List : public Object {
public:
    std::vector<std::shared_ptr<Object>> elements;
    List(std::vector<std::shared_ptr<Object>> elements) : Object(ObjectType::LIST), elements(elements) {}
};

class Map : public Object {
public:
    std::map<HashKey, MapPair> pairs;
    Map(std::map<HashKey, MapPair> pairs) : Object(ObjectType::MAP), pairs(pairs) {}
};

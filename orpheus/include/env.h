#pragma once

#include "obj.h"
#include <vector>
#include <memory>
#include <map>

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

    Entry& Get(const std::string& name);
    Type&  GetType(const std::string& name);
    Entry& Set(const std::string& name, std::shared_ptr<Object> obj, std::shared_ptr<Type> type=nullptr);
};

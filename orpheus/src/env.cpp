#include "../include/obj.h"

std::shared_ptr<Object> Environment::Get(const std::string& name) {
    auto it = store.find(name);
    if(it != store.end())
        return it->second.value;
    else if(outer != nullptr)
        return outer->Get(name);
    return nullptr;
}

std::shared_ptr<Type> Environment::GetType(const std::string& name) {
    auto it = store.find(name);
    if(it != store.end())
        return it->second.type;
    else if(outer != nullptr) 
        return outer->GetType(name);

    return nullptr;
}

std::shared_ptr<Object> Environment::Set(const std::string& name, std::shared_ptr<Object> obj, std::shared_ptr<Type> type) {
    if(outer != nullptr && outer->Get(name) != nullptr) {
        outer->Set(name, obj, type);
        return Get(name);
    }

    store[name] = Entry{obj, type};
    return Get(name);
}


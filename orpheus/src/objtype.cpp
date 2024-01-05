#include "../include/obj.h"

std::unordered_map<TokenType, ObjectType> typelit_objs = {
    {TokenType::T_INT, ObjectType::INTEGER},
    {TokenType::T_STR, ObjectType::STRING},
    {TokenType::T_FLOAT, ObjectType::FLOAT},
    {TokenType::T_BOOL, ObjectType::BOOL},
    {TokenType::T_LIST, ObjectType::LIST},
    {TokenType::T_MAP, ObjectType::MAP},
    {TokenType::T_ANY, ObjectType::ANY},
    {TokenType::FUNC, ObjectType::FUNCTION},
    {TokenType::NIL, ObjectType::NIL}
};

std::ostream& operator<<(std::ostream& out, const ObjectType value) {
    const char* str = "UNKNOWN OBJECT";
    #define PROCESS_VAL(p) case (p): str = #p; break;
    switch(value) {
        PROCESS_VAL(ObjectType::INTEGER);
        PROCESS_VAL(ObjectType::FLOAT);
        PROCESS_VAL(ObjectType::STRING);
        PROCESS_VAL(ObjectType::BOOL);
        PROCESS_VAL(ObjectType::NIL);
        PROCESS_VAL(ObjectType::RETURN_VALUE);
        PROCESS_VAL(ObjectType::ERROR);
        PROCESS_VAL(ObjectType::FUNCTION);
        PROCESS_VAL(ObjectType::BUILTIN);
        PROCESS_VAL(ObjectType::LIST);
        PROCESS_VAL(ObjectType::MAP);
        PROCESS_VAL(ObjectType::ANY);
        PROCESS_VAL(ObjectType::TYPE);

    }
    #undef PROCESS_VAL
    return out << str;
}

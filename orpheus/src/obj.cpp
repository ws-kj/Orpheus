#include "../include/obj.h"

HashKey HashObj(std::shared_ptr<Object> obj) {
    return HashKey(obj->GetType(), obj->Hash());
}

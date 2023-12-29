#include "../include/token.h"

std::ostream& operator<<(std::ostream& out, const Token& token) {
    out << token.type;
    if(!token.literal.empty() && token.literal != "\n") {
        out << " " << token.literal;
    }
    return out;
}

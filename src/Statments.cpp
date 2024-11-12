#include "AST/Statments.hpp"

std::string AST::repeat_char(char c, int times) {
    std::string s;
    for (int i = 0; i < times; i++) {
        s.push_back(c);
    }

    return s;
}
#ifndef __ANALYSER_HPP
#define __ANALYSER_HPP

#include <stack>
#include "Environnement.hpp"

class Analyser {
public:
    Analyser() {}
private:
    std::stack<Environnement> scopes;
};

#endif
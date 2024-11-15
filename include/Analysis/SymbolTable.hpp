#ifndef __SYMBOL_TABLE_HPP
#define __SYMBOL_TABLE_HPP

#include "Symbol.hpp"
#include <stack>
#include <string>
#include <unordered_map>

namespace Analysis {

using Scope = std::unordered_map<std::string, Symbol>;

class SymbolTable {
public:
  std::stack<Scope> scopes;
};

} // namespace Analysis

#endif // !__SYMBOL_TABLE_HPP

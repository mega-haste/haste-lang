#ifndef __SYMBOL_TABLE_HPP
#define __SYMBOL_TABLE_HPP

#include "Symbol.hpp"
#include <deque>
#include <optional>
#include <string>
#include <unordered_map>

namespace Analysis {

using Scope = std::unordered_map<std::string, Symbol>;

class SymbolTable {
public:
  std::deque<Scope> scopes;

  void declare(std::string &name);
  void define(std::string &name, SymbolType type);

  void scope_begin(void);
  void scope_end(void);

  std::optional<Symbol> local_first_look_up(std::string &key);

  Scope &get_current_scope(void);
};

} // namespace Analysis

#endif // !__SYMBOL_TABLE_HPP

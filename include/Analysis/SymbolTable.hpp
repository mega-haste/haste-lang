#ifndef __SYMBOL_TABLE_HPP
#define __SYMBOL_TABLE_HPP

#include "Symbol.hpp"
#include <deque>
#include <string>
#include <unordered_map>

namespace Analysis {

class SymbolTable {
private:
  void print_table(void) const;

public:
  using Scope = std::unordered_map<std::string, Symbol>;
  std::deque<Scope> scopes;

  SymbolTable();

  void declare(const std::string &name);
  void define(const std::string &name, SymbolType &&type, bool mut = false);

  void scope_begin(void);
  void scope_end(void);

  Symbol *local_first_look_up(const std::string &key);

  Scope &get_current_scope(void);
};

} // namespace Analysis

#endif // !__SYMBOL_TABLE_HPP

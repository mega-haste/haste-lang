#ifndef __SYMBOL_TABLE_HPP
#define __SYMBOL_TABLE_HPP

#include "Symbol.hpp"
#include "tokens.hpp"
#include <deque>
#include <unordered_map>

namespace Analysis {

class SymbolTable {
public:
  using Scope = std::unordered_map<Token, Symbol>;
  std::deque<Scope> scopes;

  SymbolTable();

  void print_table(void) const;

  void declare(const Token &ident);
  void define(const Token &ident, SymbolType &&type, bool mut = false);

  void scope_begin(void);
  void scope_end(void);

  Symbol *local_first_look_up(const Token &key);

  Scope &get_current_scope(void);

  ~SymbolTable() = default;
};

} // namespace Analysis

#endif // !__SYMBOL_TABLE_HPP

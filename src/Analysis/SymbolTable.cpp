#include "Analysis/SymbolTable.hpp"
#include "Analysis/Symbol.hpp"
#include "Analysis/Types.hpp"
#include "common.hpp"
#include "tokens.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <stdexcept>

namespace Analysis {

SymbolTable::SymbolTable() { scopes.push_back(Scope()); }

void SymbolTable::print_table(void) const {
  std::cout << "\n**** Started ****\n";
  int i = 0;
  for (auto &scope : scopes) {
    std::cout << "\n>>>> Scope " << i << ":\n";
    for (auto &symbol : scope) {
      std::cout << symbol.first.value << ": "
                << (IF symbol.second.is_mutable() THEN "mut" ELSE "immut")
                << " "
                << (IF symbol.second.type->is_assigned THEN "assigned" ELSE
                                                            "not_assigned")
                << " " << symbol.second.prettify() << " - ";
    }
    i++;
  }
  // std::cout << "\nEnter to continue";
  // getchar();
}

void SymbolTable::declare(const Token &ident) {
  scopes.back()[ident] =
      Symbol(NativeType::make(NativeType::Kind::Undefined), false, false);
}

void SymbolTable::define(const Token &ident, Type::Handler &&type, bool mut) {
  Scope &current_scope = get_current_scope();
  if (!current_scope.contains(ident))
    throw std::logic_error(std::format("Expected `{}` to be declared in the "
                                       "current scope. `{}` isn't declared.",
                                       ident.value, ident.value));

  Symbol &the_symbol = current_scope[ident];
  the_symbol.type = std::move(type);
  the_symbol.defined = true;
  the_symbol.type->is_mut = mut;
}

void SymbolTable::scope_begin(void) { scopes.push_back(Scope()); }
void SymbolTable::scope_end(void) {
#ifdef TABLE_PRINT
  print_table();
#endif // TABLE_PRINT
  scopes.pop_back();
}

Symbol *SymbolTable::local_first_look_up(const Token &key) {
  for (std::size_t i = scopes.size() - 1; i >= 0; i--) {
    if (scopes[i].contains(key)) {
      return &scopes[i][key];
    }
  }

  return nullptr;
}

SymbolTable::Scope &SymbolTable::get_current_scope(void) {
  return scopes.back();
}

} // namespace Analysis

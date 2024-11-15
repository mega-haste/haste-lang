#include "Analysis/SymbolTable.hpp"
#include "Analysis/Symbol.hpp"
#include <cstddef>
#include <format>
#include <optional>
#include <stdexcept>

using namespace Analysis;

void SymbolTable::declare(std::string &name) {
  scopes.back()[name] = Symbol(NativeType::Unknown, false);
}

void SymbolTable::define(std::string &name, SymbolType type) {
  Scope &current_scope = get_current_scope();
  if (!current_scope.contains(name))
    throw std::logic_error(std::format("Expected `{}` to be declared in the "
                                       "current scope. `{}` isn't declared.",
                                       name, name));

  Symbol &the_symbol = current_scope[name];
  std::get<0>(the_symbol) = type;
  std::get<1>(the_symbol) = true;
}

void SymbolTable::scope_begin(void) { scopes.push_back(Scope()); }
void SymbolTable::scope_end(void) { scopes.pop_back(); }

std::optional<Symbol> SymbolTable::local_first_look_up(std::string &key) {
  for (std::size_t i = scopes.size() - 1; i >= 0; i--) {
    if (scopes[i].contains(key)) {
      return std::make_optional<Symbol>(scopes[i][key]);
    }
  }

  return std::nullopt;
}

Scope &SymbolTable::get_current_scope(void) { return scopes.back(); }

#include "Analysis/SymbolTable.hpp"
#include "Analysis/Symbol.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>

using namespace Analysis;

SymbolTable::SymbolTable() { scopes.push_back(Scope()); }

void SymbolTable::print_table(void) const {
  std::cout << "**** Started ****\n";
  int i = 0;
  for (auto &scope : scopes) {
    std::cout << "\n>>>> Scope " << i << ":\n";
    for (auto &symbol : scope) {
      std::cout << symbol.first << ": " << (symbol.second.mut ? "mut" : "immut")
                << " " << symbol.second.prettify() << " - ";
    }
    i++;
  }
  std::cout << "\nEnter to continue";
  getchar();
}

void SymbolTable::declare(const std::string &name) {
  scopes.back()[name] =
      Symbol(std::make_shared<SymbolType>(NativeType::Unknown), false, false);
}

void SymbolTable::define(const std::string &name, SymbolType &&type, bool mut) {
  Scope &current_scope = get_current_scope();
  if (!current_scope.contains(name))
    throw std::logic_error(std::format("Expected `{}` to be declared in the "
                                       "current scope. `{}` isn't declared.",
                                       name, name));

  Symbol &the_symbol = current_scope[name];
  the_symbol.type = std::make_unique<SymbolType>(std::move(type));
  the_symbol.defined = true;
  the_symbol.mut = mut;
}

void SymbolTable::scope_begin(void) { scopes.push_back(Scope()); }
void SymbolTable::scope_end(void) {
  print_table();
  scopes.pop_back();
}

std::optional<Symbol> SymbolTable::local_first_look_up(const std::string &key) {
  for (std::size_t i = scopes.size() - 1; i > 0; i--) {
    if (scopes[i].contains(key)) {
      return std::make_optional<Symbol>(scopes[i][key]);
    }
  }

  return std::nullopt;
}

Scope &SymbolTable::get_current_scope(void) { return scopes.back(); }

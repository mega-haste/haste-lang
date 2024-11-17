#include "Analysis/Context.hpp"
#include "Analysis/Symbol.hpp"
#include <optional>

using namespace Analysis;

bool Context::is_declared(const std::string &key) {
  std::optional<Symbol> symbol = symbol_table.local_first_look_up(key);
  if (!symbol.has_value())
    return false;
  return true;
}

bool Context::is_defined(const std::string &key) {
  std::optional<Symbol> symbol = symbol_table.local_first_look_up(key);
  if (!symbol.has_value())
    return false;
  return symbol.value().defined;
}

bool Context::is_in_function(void) const {
  return current_func != FuncType::None;
}

void Context::declare(const std::string &name) { symbol_table.declare(name); }
void Context::define(const std::string &name, SymbolType &&type, bool mut) {
  symbol_table.define(name, std::move(type), mut);
}
void Context::define(const std::string &name, SymbolType &&type) {
  symbol_table.define(name, std::move(type));
}

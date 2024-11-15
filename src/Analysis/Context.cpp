#include "Analysis/Context.hpp"
#include "Analysis/Symbol.hpp"
#include <optional>

using namespace Analysis;

bool Context::is_declared(std::string &key) {
  std::optional<Symbol> symbol = symbol_table.local_first_look_up(key);
  if (!symbol.has_value())
    return false;
  return true;
}

bool Context::is_defined(std::string &key) {
  std::optional<Symbol> symbol = symbol_table.local_first_look_up(key);
  if (!symbol.has_value())
    return false;
  return std::get<1>(symbol.value());
}

#ifndef __CONTEXT_HPP
#define __CONTEXT_HPP

#include "Analysis/SymbolTable.hpp"

namespace Analysis {

class Context {
public:
  enum class ClassType { None };
  enum class FuncType { None, Function };

  ClassType current_class = ClassType::None;
  FuncType current_func = FuncType::None;

  SymbolTable symbol_table;

  bool is_declared(const std::string &key);
  bool is_defined(const std::string &key);
  bool is_in_function(void) const;

  void declare(const std::string &name);
  void define(const std::string &name, SymbolType &&type);
  void define(const std::string &name, SymbolType &&type, bool mut = false);
};

} // namespace Analysis

#endif // !__CONTEXT_HPP

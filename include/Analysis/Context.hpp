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

  bool is_declared(std::string &key);
  bool is_defined(std::string &key);
};

} // namespace Analysis

#endif // !__CONTEXT_HPP

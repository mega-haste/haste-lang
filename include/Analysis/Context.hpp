#ifndef __CONTEXT_HPP
#define __CONTEXT_HPP

#include "Analysis/Symbol.hpp"
#include "Analysis/SymbolTable.hpp"
#include <deque>

namespace Analysis {

class Context {
public:
  enum class ClassType { None };
  enum class FuncType { None, Function };

  ClassType current_class = ClassType::None;
  FuncType current_func = FuncType::None;

  std::deque<std::string> function_stack;
  SymbolTable symbol_table;

  int error_count = 0;
  int warnings_count = 0;

  std::string source_file = "main.haste";

  bool is_declared(const std::string &key);
  bool is_defined(const std::string &key);
  bool is_in_function(void) const;
  bool has_error(void) const;
  bool has_warning(void) const;

  void declare(const std::string &name);
  void define(const std::string &name, SymbolType &&type);
  void define(const std::string &name, SymbolType &&type, bool mut = false);
  void scope_begin(void);
  void scope_end(void);

  void report_error(std::size_t line, std::size_t column,
                    const std::string &title, const std::string &desctiption);
  void report_error(const Token &token, const std::string &title,
                    const std::string &desctiption);
  void report_warning(std::size_t line, std::size_t column,
                      const std::string &title, const std::string &desctiption);
  void report_warning(const Token &token, const std::string &title,
                      const std::string &desctiption);

  void report_summary(void) const;
};

} // namespace Analysis

#endif // !__CONTEXT_HPP

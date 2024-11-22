#ifndef __CONTEXT_HPP
#define __CONTEXT_HPP

#include "Analysis/Symbol.hpp"
#include "Analysis/SymbolTable.hpp"
#include "Reporter.hpp"
#include "tokens.hpp"
#include <deque>
#include <filesystem>

namespace Analysis {

class Context {
  Reporter &m_reporter;

public:
  enum class ClassType { None };
  enum class FuncType { None, Function };

  ClassType current_class = ClassType::None;
  FuncType current_func = FuncType::None;

  std::deque<std::string> function_stack;
  SymbolTable symbol_table;

  int error_count = 0;
  int warnings_count = 0;

  std::filesystem::path source_file = "main.haste";

  Context(std::filesystem::path source_file, Reporter &reporter);

  bool is_declared(const Token &key);
  bool is_defined(const Token &key);
  bool is_in_function(void) const;
  bool has_error(void) const;
  bool has_warning(void) const;

  void declare(const Token &ident);
  void define(const Token &ident, SymbolType &&type);
  void define(const Token &ident, SymbolType &&type, bool mut = false);
  Symbol *local_first_look_up(const Token &key);
  void scope_begin(void);
  void scope_end(void);

  void report_error(std::size_t line, std::size_t column, std::size_t at,
                    std::size_t length, const std::string &title,
                    const std::string &desctiption);
  void report_error(const Token &token, const std::string &title,
                    const std::string &desctiption);
  void report_warning(std::size_t line, std::size_t column, std::size_t at,
                      std::size_t length, const std::string &title,
                      const std::string &desctiption);
  void report_warning(const Token &token, char caret, const std::string &title,
                      const std::string &desctiption);
  void report_warning(const Token &token, const std::string &title,
                      const std::string &desctiption);

  void report_summary(void) const;
};

} // namespace Analysis

#endif // !__CONTEXT_HPP

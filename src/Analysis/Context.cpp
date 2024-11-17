#include "Analysis/Context.hpp"
#include "Analysis/Symbol.hpp"
#include <cstddef>
#include <format>
#include <iostream>
#include <string>

using namespace Analysis;

bool Context::is_declared(const std::string &key) {
  Symbol *symbol = symbol_table.local_first_look_up(key);
  if (symbol == nullptr)
    return false;
  return true;
}

bool Context::is_defined(const std::string &key) {
  Symbol *symbol = symbol_table.local_first_look_up(key);
  if (symbol == nullptr)
    return false;
  return symbol->defined;
}

bool Context::is_in_function(void) const {
  return current_func != FuncType::None;
}
bool Context::has_error(void) const { return error_count > 0; }
bool Context::has_warning(void) const { return warnings_count > 0; }

void Context::declare(const std::string &name) { symbol_table.declare(name); }
void Context::define(const std::string &name, SymbolType &&type, bool mut) {
  symbol_table.define(name, std::move(type), mut);
}
void Context::define(const std::string &name, SymbolType &&type) {
  symbol_table.define(name, std::move(type));
}

void Context::scope_begin(void) { symbol_table.scope_begin(); }
void Context::scope_end(void) {
  SymbolTable::Scope &current_scope = symbol_table.get_current_scope();
  for (auto &symbol : current_scope) {
    if (!symbol.second.is_used())
      report_warning(-1, -1, std::format("unused variable: `{}`", symbol.first),
                     "");
  }
  symbol_table.scope_end();
}

void Context::report_error(std::size_t line, std::size_t column,
                           const std::string &title,
                           const std::string &desctiption) {
  error_count++;
  std::cerr << "Semantic Error: " << title << "\n  in " << source_file << ":"
            << line << ":" << column << "\n"
            << desctiption << "\n";
}
void Context::report_error(const Token &token, const std::string &title,
                           const std::string &desctiption) {
  error_count++;
  std::cerr << "Semantic Error: " << title << "\n  in " << source_file << ":"
            << token.line << ":" << token.column << "\n"
            << desctiption << "\nNear to '" << token.value << "'\n";
}

void Context::report_warning(std::size_t line, std::size_t column,
                             const std::string &title,
                             const std::string &desctiption) {
  warnings_count++;
  std::cerr << "Warning: " << title << "\n  in " << source_file << ":" << line
            << ":" << column << "\n"
            << desctiption << "\n";
}
void Context::report_warning(const Token &token, const std::string &title,
                             const std::string &desctiption) {
  warnings_count++;
  std::cerr << "Warning: " << title << "\n  in " << source_file << ":"
            << token.line << ":" << token.column << "\n"
            << desctiption << "\nNear to '" << token.value << "'\n";
}

void Context::report_summary(void) const {
  if (has_error())
    std::cerr << "report summary: couldn't Compile " << source_file
              << " due to " << error_count << " previous "
              << (error_count == 1 ? "error." : "errors.\n");
  if (has_warning())
    std::cerr << "Generated " << warnings_count
              << (warnings_count == 0 ? " warning" : " warnings") << " in "
              << source_file << "\n";
}

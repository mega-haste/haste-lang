#include "Analysis/Context.hpp"
#include "Analysis/Symbol.hpp"
#include "Analysis/Types.hpp"
#include "Reporter.hpp"
#include "tokens.hpp"
#include <cstddef>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <termcolor/termcolor.hpp>

namespace Analysis {

Context::Context(std::filesystem::path source_file, Reporter &reporter)
    : m_reporter(reporter), source_file(source_file) {
  m_reporter.reseek();
}

bool Context::is_declared(const Token &key) {
  Symbol *symbol = symbol_table.local_first_look_up(key);
  if (symbol == nullptr)
    return false;
  return true;
}

bool Context::is_defined(const Token &key) {
  Symbol *symbol = local_first_look_up(key);
  if (symbol == nullptr)
    return false;
  return symbol->defined;
}

bool Context::is_in_function(void) const {
  return current_func != FuncType::None;
}
bool Context::has_error(void) const { return error_count > 0; }
bool Context::has_warning(void) const { return warnings_count > 0; }

void Context::declare(const Token &name) { symbol_table.declare(name); }
void Context::define(const Token &name, Type::Handler type, bool mut) {
  symbol_table.define(name, type, mut);
}
void Context::define(const Token &name, Type::Handler type) {
  symbol_table.define(name, type);
}

Symbol *Context::local_first_look_up(const Token &key) {
  return symbol_table.local_first_look_up(key);
}

void Context::scope_begin(void) { symbol_table.scope_begin(); }
void Context::scope_end(void) {
  SymbolTable::Scope &current_scope = symbol_table.get_current_scope();
  for (auto &symbol : current_scope) {
    if (not symbol.second.is_used()) {
      report_warning(symbol.first, '~',
                     std::format("unused variable: `{}`", symbol.first.value),
                     "This variable never been used.");
    }
  }
  symbol_table.scope_end();
}

void Context::report_error(std::size_t line, std::size_t column, std::size_t at,
                           std::size_t length, const std::string &title,
                           const std::string &desctiption) {
  error_count++;
  m_reporter.report(ReporterType::SemanticError, title,
                    {ReportLocation(line, column, at, length, desctiption)});
}
void Context::report_error(const Token &token, const std::string &title,
                           const std::string &desctiption) {
  error_count++;
  m_reporter.report(ReporterType::SemanticError, title,
                    {ReportLocation(token.line, token.column, token.at,
                                    token.length, desctiption)});
}

void Context::report_warning(std::size_t line, std::size_t column,
                             std::size_t at, std::size_t length,
                             const std::string &title,
                             const std::string &desctiption) {
  warnings_count++;
  m_reporter.report(ReporterType::Warning, title,
                    {ReportLocation(line, column, at, length, desctiption)});
}
void Context::report_warning(const Token &token, char caret,
                             const std::string &title,
                             const std::string &desctiption) {
  warnings_count++;
  m_reporter.report(ReporterType::Warning, title,
                    {ReportLocation(token.line, token.column, token.at,
                                    token.length, caret, desctiption)});
}
void Context::report_warning(const Token &token, const std::string &title,
                             const std::string &desctiption) {
  warnings_count++;
  m_reporter.report(ReporterType::Warning, title,
                    {ReportLocation(token.line, token.column, token.at,
                                    token.length, desctiption)});
}

void Context::report_summary(void) const {
  if (has_error() or has_warning())
    std::cerr << termcolor::blue << termcolor::bold << "Report summary"
              << termcolor::reset << termcolor::bold << ": ";
  if (has_error())
    std::cerr << "Couldn't Compile " << termcolor::reset << termcolor::underline
              << source_file << termcolor::reset << termcolor::bold
              << " due to " << termcolor::red << error_count << " previous "
              << (error_count == 1 ? "error" : "errors") << termcolor::reset
              << ".\n";
  if (has_warning())
    std::cerr << termcolor::bold << "Generated " << termcolor::yellow
              << warnings_count
              << (warnings_count == 1 ? " warning" : " warnings")
              << termcolor::reset << termcolor::bold << " in "
              << termcolor::reset << termcolor::underline << source_file
              << termcolor::reset << ".\n";
}

} // namespace Analysis

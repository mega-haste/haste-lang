#include "TranslationUnit.hpp"

TranslationUnit::TranslationUnit(std::filesystem::path &path)
    : source_path(path), ctx(path) {
  source_path = path;
  m_scanner = Scanner(source_path);
}

void TranslationUnit::do_scan() { m_tokens = m_scanner.scan(); }

void TranslationUnit::add_function(std::unique_ptr<AST::FunctionDef> func) {
  functions.push_back(std::move(func));
}

void TranslationUnit::pretty_print(void) const {
  for (auto &function : functions) {
    std::cout << function->prettify() << '\n';
  }
}

void TranslationUnit::analyse() {
  for (auto &fn : functions) {
    fn->analyse(ctx);
  }

  ctx.report_summary();
}
const TokenList &TranslationUnit::get_tokens() const { return m_tokens; }

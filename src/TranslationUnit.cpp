#include "TranslationUnit.hpp"

TranslationUnit::TranslationUnit(std::filesystem::path &path)
    : source_path(path), ctx(path) {
  source_path = path;
  m_scanner = Scanner(source_path);
}

void TranslationUnit::do_scan() { m_tokens = m_scanner.scan(); }

void TranslationUnit::analyse() {
  for (auto &fn : program.functions) {
    fn->analyse(ctx);
  }

  ctx.report_summary();
}
const TokenList &TranslationUnit::get_tokens() const { return m_tokens; }

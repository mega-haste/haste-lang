#include "TranslationUnit.hpp"
#include "Reporter.hpp"
#include "Scanner.hpp"
#include <cstdio>
#include <ios>

TranslationUnit::TranslationUnit(std::filesystem::path path)
    : m_reporter(m_file, path.string()), source_path(path),
      ctx(path, m_reporter) {
  m_file.open(path);
  m_scanner = Scanner(source_path);
}

void TranslationUnit::do_scan() { m_tokens = m_scanner.scan(); }

void TranslationUnit::analyse() {
  m_file.seekg(0, std::ios::beg);
  for (auto &fn : program.functions) {
    fn->analyse(ctx);
  }

  ctx.report_summary();
}
const TokenList &TranslationUnit::get_tokens() const { return m_tokens; }

#include "TranslationUnit.hpp"
#include "Reporter.hpp"
#include "Scanner.hpp"
#include "common.hpp"
#include <cstdio>
#include <ios>
#include <string_view>

TranslationUnit::TranslationUnit(std::filesystem::path path)
    : source_path(path), ctx(path, m_reporter) {
  m_file.open(path);

  m_file.seekg(0, std::ios::end);
  std::streamsize size = m_file.tellg();
  m_file.seekg(0, std::ios::beg);

  m_content.reserve(size);
  m_content.resize(size);

  if (not m_file.read((char *)m_content.c_str(), size)) {
    LOG("Can't compiler");
    return;
  }

  std::string_view content_view(m_content.c_str(),
                                m_content.c_str() + m_content.size());
  m_reporter = Reporter(content_view, &m_lines, path);

  m_scanner = Scanner(
      std::string_view(m_content.c_str(), m_content.c_str() + m_content.size()),
      &m_lines);
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

#include "Compiler.hpp"
#include "Parser.hpp"
#include "TranslationUnit.hpp"
#include "macros.hpp"
#include "tokens.hpp"
#include <memory>
#include <string>

void Compiler::compile(std::filesystem::path path) {
  std::shared_ptr<TranslationUnit> tu = make_tu(path);
  tu->do_scan();

  const TokenList &tokens = tu->get_tokens();
  for (auto &token : tokens) {
    LOG(token.to_string());
  }

  Parser parser(tokens);
  parser.parse(tu);
}

void Compiler::analyse_all() {
  for (auto &tu : m_tus) {
    tu.second->analyse();
  }
}

std::shared_ptr<TranslationUnit> Compiler::make_tu(std::filesystem::path path) {
  std::string path_str = path.string();
  m_tus[path_str] = std::make_shared<TranslationUnit>(path);
  return m_tus[path_str];
}

#ifndef __TRANSLATION_UNIT__CPP
#define __TRANSLATION_UNIT__CPP
#include "AST/Statments.hpp"
#include "Scanner.hpp"
#include "tokens.hpp"
#include <filesystem>
#include <fstream>

class TranslationUnit {
  std::ifstream m_file;
  Scanner m_scanner;
  TokenList m_tokens;

public:
  std::vector<std::unique_ptr<AST::FunctionDef>> functions;
  std::filesystem::path &source_path;
  Analysis::Context ctx;

  TranslationUnit(std::filesystem::path &path);

  void do_scan();
  void add_function(std::unique_ptr<AST::FunctionDef> func);
  void pretty_print(void) const;
  void analyse();

  const TokenList &get_tokens() const;
};

#endif

#ifndef __TRANSLATION_UNIT__CPP
#define __TRANSLATION_UNIT__CPP

#include "Program.hpp"
#include "Scanner.hpp"
#include "tokens.hpp"
#include <filesystem>
#include <fstream>

class TranslationUnit {
  std::ifstream m_file;
  Scanner m_scanner;
  TokenList m_tokens;

public:
  Program program;
  std::filesystem::path &source_path;
  Analysis::Context ctx;

  TranslationUnit(std::filesystem::path &path);

  void do_scan();
  void analyse();

  const TokenList &get_tokens() const;
};

#endif

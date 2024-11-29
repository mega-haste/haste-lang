#ifndef __TRANSLATION_UNIT__CPP
#define __TRANSLATION_UNIT__CPP

#include "Program.hpp"
#include "Reporter.hpp"
#include "Scanner.hpp"
#include "tokens.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

class TranslationUnit {
  std::ifstream m_file;
  std::string m_content;
  Reporter m_reporter;
  Scanner m_scanner;
  TokenList m_tokens;
  std::vector<std::string_view> m_lines;

public:
  Program program;
  std::filesystem::path source_path;
  Analysis::Context ctx;

  TranslationUnit(std::filesystem::path path);

  void do_scan();
  void analyse();

  const TokenList &get_tokens() const;
};

#endif

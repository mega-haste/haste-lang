#ifndef __COMPILER_HPP
#define __COMPILER_HPP

#include "TranslationUnit.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

class Compiler {
public:
  void compile(std::filesystem::path path);
  void analyse_all();

private:
  std::unordered_map<std::string, std::shared_ptr<TranslationUnit>> m_tus;

  std::shared_ptr<TranslationUnit> make_tu(std::filesystem::path path);
};

#endif // !__COMPILER_HPP

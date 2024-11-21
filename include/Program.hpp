#ifndef __PROGRAM_HPP
#define __PROGRAM_HPP

#include "AST/Statments.hpp"
#include <memory>
#include <vector>

class Program {
public:
  std::vector<std::unique_ptr<AST::FunctionDef>> functions;

  void add_function(std::unique_ptr<AST::FunctionDef> func);
  void pretty_print(void) const;
};

#endif // !__PROGRAM_HPP

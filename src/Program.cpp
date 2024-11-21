#include "Program.hpp"

void Program::add_function(std::unique_ptr<AST::FunctionDef> func) {
  functions.push_back(std::move(func));
}

void Program::pretty_print(void) const {
  for (auto &function : functions) {
    std::cout << function->prettify() << '\n';
  }
}

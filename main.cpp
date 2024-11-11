#include "Parser.hpp"
#include "Scanner.hpp"
#include "tokens.hpp"
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
  while (true) {
    std::string input;

    std::cout << "haste> ";
    getline(std::cin, input);

    Scanner scanner(input);
    TokenList tokens = scanner.scan();

    Parser parser(tokens);
    auto p = parser.parse();

    for (auto &s : p) {
      std::cout << (*s).prettify() << std::endl;
    }
  }
  return 0;
}

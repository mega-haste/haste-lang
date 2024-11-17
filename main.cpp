#include "Parser.hpp"
#include "Scanner.hpp"
#include "tokens.hpp"
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>

int main(int argc, char *argv[]) {
  std::ifstream file("main.haste");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content = buffer.str();

  // std::string content = "func main() {\
  //     if (func) {}\
  //   }";

  Scanner scanner(content);
  TokenList tokens = scanner.scan();

  Parser parser(tokens);
  auto res = parser.parse();

  // res.pretty_print();
  res.analyse();

  std::cout << std::endl;
  return 0;
}

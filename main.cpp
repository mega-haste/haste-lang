#include "Parser.hpp"
#include "Scanner.hpp"
#include "tokens.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
    std::ifstream file("main.haste");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    Scanner scanner(content);
    TokenList tokens = scanner.scan();

    Parser parser(tokens);
    auto res = parser.parse();

    res.pretty_print();

    return 0;
}


#include "Parser.hpp"
#include "macros.hpp"

std::vector<Statement> Parser::block(void) {
  std::vector<Statement> statements;

  while (!check(TokenType::CloseCurlyBrase) && !is_at_end()) {
    statements.push_back(declaration());
  }

  consume(TokenType::CloseCurlyBrase, "Expected '}' as end of a code block.");
  return statements;
}

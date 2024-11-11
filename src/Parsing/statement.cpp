#include "Parser.hpp"

Statement Parser::statement(void) {
  if (match({TokenType::OpenCurlyBrase}))
    return std::make_unique<BlockStatement>(std::move(block()));
  return expression_statement();
}

Statement Parser::expression_statement(void) {
  auto expr = expression();
  consume_semi_colon();
  return std::make_unique<ExpressionStatement>(std::move(expr));
}

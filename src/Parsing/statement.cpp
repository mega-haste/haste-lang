#include "Parser.hpp"

Statement Parser::statement(void) {
  if (match({TokenType::Return})) return $return();
  if (match({TokenType::If})) return if_condition();
  if (match({TokenType::OpenCurlyBrase})) return std::make_unique<BlockStatement>(std::move(block()));
  
  return expression_statement();
}

Statement Parser::expression_statement(void) {
  auto expr = expression();
  consume_semi_colon("Expected ';' at the end of the expression.");
  return std::make_unique<ExpressionStatement>(std::move(expr));
}

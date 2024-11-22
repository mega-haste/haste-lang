#include "AST/Statments.hpp"
#include "Parser.hpp"
#include "tokens.hpp"

Statement Parser::statement(void) {
  if (match({TokenType::Return}))
    return $return();
  if (match({TokenType::If}))
    return if_condition();
  if (match({TokenType::OpenCurlyBrase}))
    return block_statement();

  return expression_statement();

  return std::make_unique<NoneStmt>();
}

Statement Parser::block_statement(void) {
  const Token &starting_token = previous();

  auto res = std::make_unique<BlockStatement>(block());
  res->start = starting_token;
  res->end = previous();

  return res;
}

Statement Parser::expression_statement(void) {
  const Token &starting_token = previous();

  auto expr = expression();

  ExpressionStatement result(std::move(expr));
  result.start = starting_token;
  result.end = consume_semi_colon("Expected ';' at the end of the expression.");

  return std::make_unique<ExpressionStatement>(std::move(result));
}

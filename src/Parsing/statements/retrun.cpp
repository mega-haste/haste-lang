#include "Parser.hpp"
#include "tokens.hpp"

Statement Parser::$return(void) {
  const Token &starting_token = previous();

  std::optional<Expression> result_expression =
      !check(TokenType::SemiColon) ? std::optional{expression()} : std::nullopt;

  auto res = std::make_unique<ReturnStatement>(std::move(result_expression));
  res->start = starting_token;
  res->end =
      consume_semi_colon("Expected ';' at the end of 'return' statement");

  return res;
}

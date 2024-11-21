#include "Parser.hpp"

Statement Parser::$return(void) {
  std::optional<Expression> res =
      !check(TokenType::SemiColon) ? std::optional{expression()} : std::nullopt;
  consume_semi_colon("Expected ';' at the end of 'return' statement");
  return std::make_unique<ReturnStatement>(std::move(res));
}

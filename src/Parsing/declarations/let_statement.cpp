
#include "AST/Statments.hpp"
#include "Parser.hpp"
#include "tokens.hpp"

#include <optional>

Statement Parser::let(void) {
  const Token &starting_token = previous();

  bool mut = match({TokenType::Mut});
  const Token &name =
      consume_identifier("Expected identifier of let statement.");
  std::optional<Type> type =
      match({TokenType::Colon}) ? std::optional{parse_type()} : std::nullopt;
  std::optional<Expression> value =
      match({TokenType::Eq}) ? std::optional{expression()} : std::nullopt;

  auto res =
      std::make_unique<LetDef>(name, std::move(value), std::move(type), mut);
  res->start = starting_token;
  res->end = consume_semi_colon("Expected ';' at the end of let statement.");

  return res;
}

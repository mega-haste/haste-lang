#include "AST/Expressions.hpp"
#include "AST/Statments.hpp"
#include "Parser.hpp"
#include "tokens.hpp"

using namespace AST;

Statement Parser::if_condition(void) {
  const Token &starting_token = previous();

  consume(TokenType::OpenParen, "Expected '(' after 'if'.");
  Expression condition = expression();
  consume(TokenType::CloseParen, "Expected ')' as closing of 'if ('.");
  auto then = declaration();
  std::optional<Statement> otherwise =
      match({TokenType::Else}) ? std::optional{declaration()} : std::nullopt;

  auto res = std::make_unique<IfStatement>(
      std::move(condition), std::move(then), std::move(otherwise));
  res->start = starting_token;
  res->end = previous();

  return res;
}

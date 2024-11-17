#include "AST/Expressions.hpp"
#include "AST/Statments.hpp"
#include "Parser.hpp"

using namespace AST;

Statement Parser::if_condition(void) {
  consume(TokenType::OpenParen, "Expected '(' after 'if'.");
  Expression condition = expression();
  consume(TokenType::CloseParen, "Expected ')' as closing of 'if ('.");
  auto then = declaration();
  std::optional<Statement> otherwise =
      match({TokenType::Else}) ? std::optional{declaration()} : std::nullopt;

  return std::make_unique<IfStatement>(std::move(condition), std::move(then),
                                       std::move(otherwise));
}

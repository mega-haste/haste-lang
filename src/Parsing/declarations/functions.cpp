#include "AST/TypeNode.hpp"
#include "Parser.hpp"

std::unique_ptr<FunctionDef> Parser::function(void) {
  const Token &starting_token = previous();
  const Token &name = consume(TokenType::Identifier, "Expected function name.");

  consume(TokenType::OpenParen, "Expected '(' after function name.");
  std::vector<TypedIdentifier> args;
  if (!check(TokenType::CloseParen)) {
    do {
      if (args.size() > MAX_PRAM_COUT)
        throw error(peek(), "Cannot have more than 8 parameters.");

      args.push_back(parse_typed_ident());
    } while (match({TokenType::Comma}));
  }
  consume(TokenType::CloseParen, "Expected ')' after parameters.");

  TypeNode::Handler return_type = std::make_unique<TypeLiteral>(f);
  if (match({TokenType::Colon})) {
    return_type = parse_type();
  }

  Statement body;
  if (match({TokenType::OpenCurlyBrase})) {
    const Token &block_starting_token = previous();

    body = block_statement();
    body->start = block_starting_token;
    body->end = previous();
  } else if (match({TokenType::Eq})) {
    body = expression_statement();
  } else {
    throw error(peek(), "Expected either '=' or '{'.");
  }

  FunctionDef result(name, std::move(args), std::move(return_type),
                     std::move(body));
  result.start = starting_token;
  result.end = previous();
  return std::make_unique<FunctionDef>(std::move(result));
}

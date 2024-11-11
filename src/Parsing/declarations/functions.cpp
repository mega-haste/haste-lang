#include "Parser.hpp"


Statement Parser::function(void) {
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

  Type return_type = std::make_unique<TypeLiteral>(f);
  if (match({TokenType::Colon})) {
    return_type = parse_type();
  }

  consume(TokenType::OpenCurlyBrase, "Expected '{' as starting of a function body.");
  auto body = block();

  return std::make_unique<FunctionDef>(
    name, std::move(args),
    std::move(return_type), std::move(body)
  );
}
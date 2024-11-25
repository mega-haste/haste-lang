#include "AST/TypeNode.hpp"
#include "Parser.hpp"
#include "tokens.hpp"

TypedIdentifier Parser::parse_typed_ident(void) {
  if (!match({TokenType::Identifier, TokenType::SpicialIdentifier})) {
    throw error(previous(), "Expected identifier.");
  }
  const Token &ident = previous(); // This is same as the starting token

  consume(TokenType::Colon, "Expected ':' after the identifier.");
  TypeNode::Handler type = parse_type();

  auto res = TypedIdentifier(ident, std::move(type));
  res.start = ident;
  res.end = previous();

  return res;
}

TypeNode::Handler Parser::parse_type(void) {
  if (match({TokenType::Int, TokenType::UInt, TokenType::Float,
             TokenType::String, TokenType::Void, TokenType::Bool,
             TokenType::Auto, TokenType::Char})) {
    const Token &token = previous(); // Again, this is the starting token
    if (match({TokenType::OpenSquareBracket})) {
      std::optional<Token> size = match({TokenType::IntLit, TokenType::Auto})
                                      ? std::optional(previous())
                                      : std::nullopt;
      auto res = std::make_unique<ArrayType>(
          std::make_unique<TypeLiteral>(token), size);
      res->start = token;
      res->end = consume(TokenType::CloseSquareBracket,
                         "Exprected closing bracket ']'.");
      return res;
    }

    auto res = std::make_unique<TypeLiteral>(token);
    res->start = token;
    res->end = previous();

    return std::move(res);
  }
  auto res = std::make_unique<UndefinedType>();
  res->start = previous();
  res->end = previous();

  return res;
}

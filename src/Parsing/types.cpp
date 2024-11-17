#include "AST/TypeNode.hpp"
#include "Parser.hpp"
#include "tokens.hpp"

TypedIdentifier Parser::parse_typed_ident(void) {
  if (!match({TokenType::Identifier, TokenType::SpicialIdentifier})) {
    throw error(previous(), "Expected identifier.");
  }
  const Token &ident = previous();

  consume(TokenType::Colon, "Expected ':' after the identifier.");
  Type type = parse_type();

  return TypedIdentifier(ident, std::move(type));
}

Type Parser::parse_type(void) {
  if (match({TokenType::Int, TokenType::UInt, TokenType::Float,
             TokenType::String, TokenType::Void, TokenType::Bool,
             TokenType::Auto, TokenType::Char})) {
    const Token &token = previous();
    if (match({TokenType::OpenSquareBracket})) {
      std::optional<Token> size =
          match({TokenType::IntLit}) ? std::optional(previous()) : std::nullopt;
      consume(TokenType::CloseSquareBracket, "Exprected closing bracket ']'.");
      return std::make_unique<ArrayType>(std::make_unique<TypeLiteral>(token),
                                         size);
    }
    return std::make_unique<TypeLiteral>(token);
  }

  return std::make_unique<UndefinedType>();
}

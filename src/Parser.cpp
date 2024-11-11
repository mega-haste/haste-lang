#include "Parser.hpp"
#include "AST/Expressions.hpp"
#include "tokens.hpp"
#include "AST/sigma.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

using namespace AST;

ParserError::ParserError(Token token, const char *message)
    : token(token), message(message) {}

Parser::Parser(TokenList tokens) : m_tokens(tokens) {}

std::vector<Statement> Parser::parse() {
  std::vector<Statement> statements;
  while (!is_at_end()) {
    statements.push_back(declaration());
  }

  return statements;
}

Statement Parser::declaration(void) {
  try {
    if (match({TokenType::Func}))
      return function();

    return statement();
  } catch (ParserError &e) {
    synchronize();
    return nullptr;
  }
}

Statement Parser::statement(void) {
  return expression_statement();
}

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

std::vector<Statement> Parser::block(void) {
  std::vector<Statement> statements;

  while (!check(TokenType::CloseCurlyBrase) && !is_at_end()) {
    statements.push_back(declaration());
  }

  consume(TokenType::CloseCurlyBrase, "Expected '}' as end of a code block.");
  return statements;
}

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
  if (match({TokenType::Int, TokenType::UInt, TokenType::Float, TokenType::String, TokenType::Void}))
  {
    const Token &token = previous();
    if (match({TokenType::OpenSquareBracket})) {
      std::optional<Token> size = match({TokenType::IntLit}) ? std::optional(previous()) : std::nullopt;
      consume(TokenType::CloseSquareBracket, "Exprected closing bracket ']'.");
      return std::make_unique<ArrayType>(
        std::make_unique<TypeLiteral>(token),
        size
      );
    }
    return std::make_unique<TypeLiteral>(token);
  }

  return std::make_unique<UndefinedType>();
}

Statement Parser::expression_statement(void) {
  auto expr = expression();
  consume_semi_colon();
  return std::make_unique<ExpressionStatement>(std::move(expr));
}

Expression Parser::expression(void) { return assignment(); }

Expression Parser::assignment(void) {
  auto left = inline_if();
  return left;
}

Expression Parser::inline_if(void) {
  if (match({TokenType::If})) {
    auto condition = expression();

    consume(TokenType::Then,
            "Expected `then` after the condition of inline if.");
    auto consequent = expression();

    consume(TokenType::Else, "Expected `else` after the `then` cluase.");
    auto alternate = expression();

    return std::make_unique<InlineIf>(
        std::move(condition), std::move(consequent), std::move(alternate));
  }

  return as();
}

Expression Parser::as(void) {
  auto lhs = $or();

  while (match({TokenType::As})) {
    auto type = parse_type();
    lhs = std::make_unique<AsExpression>(std::move(lhs), std::move(type));
  }

  return lhs;
}

Expression Parser::$or(void) {
  auto left = $and();

  while (match({TokenType::Or})) {
    const Token &op = previous();
    auto right = $and();
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

Expression Parser::$and(void) {
  auto left = equality();

  while (match({TokenType::And})) {
    const Token &op = previous();
    auto right = equality();
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

Expression Parser::equality(void) {
  auto left = relational();

  while (match({TokenType::EqEq, TokenType::BangEq})) {
    const Token &op = previous();
    auto right = relational();
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

Expression Parser::relational(void) {
  auto left = bitwise_shift();

  while (match({TokenType::LessThan, TokenType::LessThanOrEq,
                TokenType::GreaterThan, TokenType::GreaterThanOrEq})) {
    const Token &op = previous();
    auto right = bitwise_shift();
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

Expression Parser::bitwise_shift(void) {
  auto left = addition();

  while (match({TokenType::BitWiseLeftShift, TokenType::BitWiseRightShift})) {
    const Token &op = previous();
    auto right = addition();
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

Expression Parser::addition(void) {
  auto left = multiplication();

  while (match({TokenType::Plus, TokenType::Minus})) {
    const Token &op = previous();
    auto right = multiplication();
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

Expression Parser::multiplication(void) {
  auto left = power();

  while (match({TokenType::Star, TokenType::FSlash, TokenType::PercentSign})) {
    const Token &op = previous();
    auto right = power();
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

Expression Parser::power(void) {
  auto left = nuts();

  while (match({TokenType::DoubleStar})) {
    const Token &op = previous();
    auto right = nuts(); // 🥜
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

// 🥜
Expression Parser::nuts(void) {
  if (match({TokenType::Not, TokenType::BitwiseNot})) {
    const Token &op = previous();
    auto right = nuts(); // 🥜
    return std::make_unique<UnaryExpression>(op, std::move(right));
  }

  return unary();
}

Expression Parser::unary(void) {
  if (match({TokenType::Minus, TokenType::Plus})) {
    const Token &op = previous();
    auto right = unary();
    return std::make_unique<UnaryExpression>(op, std::move(right));
  }

  return primary();
}

Expression Parser::primary(void) {
  if (match({TokenType::True}))
    return std::make_unique<BooleanExpression>(true);
  if (match({TokenType::False}))
    return std::make_unique<BooleanExpression>(false);

  if (match({TokenType::CharLit, TokenType::StringLit, TokenType::IntLit,
             TokenType::FloatLit}))
    return std::make_unique<LiteralExpression>(previous());

  if (match({TokenType::Identifier, TokenType::SpicialIdentifier}))
    return std::make_unique<IdentifierExpression>(previous());

  if (match({TokenType::OpenParen})) {
    auto expr = expression();
    consume(TokenType::CloseParen, "Expected ')' after expression.");
    return std::make_unique<GroupingExpression>(std::move(expr));
  }

  if (match({TokenType::If})) {
    m_current--;
    return expression();
  }

  throw error(peek(), "Expected expression.");
}

const bool Parser::match(std::initializer_list<TokenType> types) {
  for (TokenType type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

const bool Parser::check(TokenType type) const {
  return is_at_end() ? false : peek() == type;
}

const Token &Parser::consume(TokenType type, const char *message) {
  if (check(type))
    return advance();

  throw error(peek(), message);
}

const bool Parser::is_at_end(void) const { return peek() == TokenType::_EOF; }

const Token &Parser::consume_semi_colon(void) {
  return consume(TokenType::SemiColon, "Expected ';' after end of a statment.");
}

const Token &Parser::peek(void) const { return m_tokens.at(m_current); }
const Token &Parser::peek(int ahead) const { return m_tokens.at(m_current + ahead); }

const Token &Parser::previous(void) const { return m_tokens.at(m_current - 1); }

const Token &Parser::advance(void) {
  if (!is_at_end())
    m_current++;
  return previous();
}

ParserError Parser::error(Token token, const char *message) {
  return ParserError(token, message);
}

void Parser::synchronize(void) {
  advance();
  // TODO
}

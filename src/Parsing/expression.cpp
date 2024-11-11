#include "Parser.hpp"
#include "tokens.hpp"

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
  auto left = bitwise_and();

  while (match({TokenType::And})) {
    const Token &op = previous();
    auto right = bitwise_and();
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

Expression Parser::bitwise_and(void) {
  auto left = bitwise_or();

  while (match({TokenType::BitwiseAnd})) {
    const Token &op = previous();
    auto right = bitwise_or();
    left = std::make_unique<BinaryExpression>(std::move(left), op,
                                              std::move(right));
  }

  return left;
}

Expression Parser::bitwise_or(void) {
  auto left = equality();

  while (match({TokenType::BitwiseOr})) {
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

  return call();
}

Expression Parser::call(void) {
  auto expr = primary();

  while (true) {
    if (match({TokenType::OpenParen})) {
      expr = finish_call(std::move(expr));
    } else if (match({TokenType::Dot})) {
      const Token &name = consume_identifier("Expected property name after the dot '.'.");
      expr = std::make_unique<MemberAccessExpression>(
        std::move(expr),
        previous(),
        name
      );
    } else {
      break;
    }
  }

  return expr;
}

Expression Parser::finish_call(Expression &&callee) {
  std::vector<Expression> arguments;
    if (!check(TokenType::CloseParen)) {
      do {
        if (arguments.size() > MAX_PRAM_COUT) {
          error(peek(), "Cannot have that much of arguments");
        }

        arguments.push_back(expression());
      } while (match({TokenType::Comma}));
    }

    const Token &paren = consume(TokenType::CloseParen, "Expect ')' after arguments.");

    return std::make_unique<CallExpression>(
      std::move(callee),
      paren,
      std::move(arguments)
    );
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

#include "AST/Expressions.hpp"
#include "Parser.hpp"
#include "tokens.hpp"
#include <memory>

Expression Parser::expression(void) {
  const Token &starting_token = peek();

  Expression result = assignment();
  result->start = starting_token;
  result->end = previous();

  return result;
}

Expression Parser::assignment(void) {
  auto left = inline_if();
  while (match({TokenType::Eq, TokenType::PlusEq, TokenType::MinusEq,
                TokenType::StarEq, TokenType::FSlashEq, TokenType::DoubleStarEq,
                TokenType::PercentSignEq})) {
    const Token &eq = previous();
    auto value = expression();

    left = std::make_unique<AssignExpression>(std::move(left), eq,
                                              std::move(value));
    left->end = previous();
  }
  return left;
}

Expression Parser::inline_if(void) {
  if (match({TokenType::If})) {
    const Token &start_token = previous();

    auto condition = expression();

    consume(TokenType::Then,
            "Expected `then` after the condition of inline if.");
    auto consequent = expression();

    consume(TokenType::Else, "Expected `else` after the `then` cluase.");
    auto alternate = expression();

    InlineIf result(std::move(condition), std::move(consequent),
                    std::move(alternate));
    result.start = start_token;
    result.end = previous();

    return std::make_unique<InlineIf>(std::move(result));
  }

  return as();
}

Expression Parser::as(void) {
  auto lhs = $or();

  while (match({TokenType::As})) {
    auto type = parse_type();
    lhs = std::make_unique<AsExpression>(std::move(lhs), std::move(type));
    lhs->end = previous();
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
    left->end = previous();
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
    left->end = previous();
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
    left->end = previous();
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
    left->end = previous();
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
    left->end = previous();
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
    left->end = previous();
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
    left->end = previous();
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
    left->end = previous();
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
    left->end = previous();
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
    left->end = previous();
  }

  return left;
}

// 🥜
Expression Parser::nuts(void) {
  if (match({TokenType::Not, TokenType::BitwiseNot})) {
    const Token &op = previous();
    auto right = nuts(); // 🥜
    auto res = std::make_unique<UnaryExpression>(op, std::move(right));
    res->start = op;
    res->end = previous();
    return res;
  }

  return unary();
}

Expression Parser::unary(void) {
  if (match({TokenType::Minus, TokenType::Plus})) {
    const Token &op = previous();
    auto right = unary();
    auto res = std::make_unique<UnaryExpression>(op, std::move(right));
    res->start = op;
    res->end = previous();
    return res;
  }

  return call();
}

Expression Parser::call(void) {
  auto expr = scope_resolution();

  while (true) {
    if (match({TokenType::OpenParen})) {
      expr = finish_call(std::move(expr));
      expr->end = previous();
    } else if (match({TokenType::OpenSquareBracket})) {
      const Token &open_bracket = previous();
      auto index = expression();
      const Token &close_bracket = consume(TokenType::CloseSquareBracket,
                                           "Expected closing bracket ']'.");
      expr = std::make_unique<SubscriptingExpression>(
          std::move(expr), open_bracket, close_bracket, std::move(index));
      expr->end = previous();
    } else if (match({TokenType::Dot})) {
      const Token &name =
          consume_identifier("Expected property name after the dot '.'.");
      expr = std::make_unique<MemberAccessExpression>(std::move(expr),
                                                      previous(), name);
      expr->end = previous();
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

  const Token &paren =
      consume(TokenType::CloseParen, "Expect ')' after arguments.");

  return std::make_unique<CallExpression>(std::move(callee), paren,
                                          std::move(arguments));
}

Expression Parser::scope_resolution(void) {
  auto left = primary();

  while (match({TokenType::ColonColon})) {
    const Token &colon_colon = previous();
    const Token &ident = consume_identifier("Expected an identifier here.");
    left = std::make_unique<ScopeResolutionExpression>(std::move(left),
                                                       colon_colon, ident);
    left->end = previous();
  }

  return left;
}

Expression Parser::primary(void) {
  const Token &starting_token = peek();
  if (match({TokenType::True})) {
    auto res = std::make_unique<BooleanExpression>(true);

    res->start = starting_token;
    res->end = previous();

    return res;
  }
  if (match({TokenType::False})) {
    auto res = std::make_unique<BooleanExpression>(false);

    res->start = starting_token;
    res->end = previous();

    return res;
  }

  if (match({TokenType::CharLit, TokenType::StringLit, TokenType::IntLit,
             TokenType::FloatLit})) {
    auto res = std::make_unique<LiteralExpression>(previous());

    res->start = starting_token;
    res->end = previous();

    return res;
  }

  if (match({TokenType::Identifier, TokenType::SpicialIdentifier})) {
    auto res = std::make_unique<IdentifierExpression>(previous());

    res->start = starting_token;
    res->end = previous();

    return res;
  }

  if (match({TokenType::OpenParen})) {
    std::vector<Expression> exprs;
    exprs.push_back(expression());
    if (match({TokenType::Comma})) {
      auto res = parse_tuple(std::move(exprs));
      res->start = starting_token;
      return res;
    }

    consume(TokenType::CloseParen, "Expected ')' after expression.");
    auto res = std::make_unique<GroupingExpression>(std::move(exprs[0]));

    res->start = starting_token;
    res->end = previous();

    return res;
  }

  if (match({TokenType::OpenSquareBracket})) {
    return parse_array();
  }

  if (match({TokenType::If})) {
    m_current--;
    return inline_if();
  }

  throw error(peek(), "Expected expression.");
}

Expression Parser::parse_tuple(std::vector<Expression> &&exprs) {
  do {
    if (exprs.size() > MAX_TUPLE_COUT) {
      throw error(peek(), "Too much elements on this tuple. (> 60)");
    }

    exprs.push_back(expression());
  } while (match({TokenType::Comma}));

  const Token &ending_token =
      consume(TokenType::CloseParen, "Expected ')' after expression.");
  auto res = std::make_unique<TupleExpression>(std::move(exprs));
  res->end = ending_token;
  return res;
}

Expression Parser::parse_array() {
  std::vector<Expression> exprs;
  do {
    exprs.push_back(expression());
  } while (match({TokenType::Comma}));

  const Token &ending_token =
      consume(TokenType::CloseSquareBracket, "Expected ']' after expression.");
  auto res = std::make_unique<ArrayExpression>(std::move(exprs));
  res->end = ending_token;
  return res;
}

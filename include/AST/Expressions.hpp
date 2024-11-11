#pragma once

#include "tokens.hpp"
#include <cmath>
#include <cstdlib>
#include <format>
#include <memory>
#include <string>
#include "TypeNode.hpp"

namespace AST {

class ExpressionNode {
public:
  virtual std::string prettify(void) const = 0;
};

using Expression = std::unique_ptr<ExpressionNode>;

class LiteralExpression : public ExpressionNode {
public:
  const Token &value;

  explicit LiteralExpression(const Token &v) : value(v) {}

  std::string prettify(void) const override {
    switch (value.type) {
    case TokenType::CharLit:
      return std::format("(char \'{}\')", value.value);
    case TokenType::StringLit:
      return std::format("(string \"{}\")", value.value);
    case TokenType::IntLit:
      return std::format("(int {})", value.value);
    case TokenType::FloatLit:
      return std::format("(float {})", value.value);
    default:
      std::exit(70);
    }
  }
};

class BooleanExpression : public ExpressionNode {
public:
  bool value;

  explicit BooleanExpression(bool v) : value(v) {}

  std::string prettify(void) const override {
    return value ? "(bool true)" : "(bool false)";
  }
};

class UnaryExpression : public ExpressionNode {
public:
  const Token &op;
  Expression rhs;

  explicit UnaryExpression(const Token &op, Expression&& rhs)
      : op(op), rhs(std::move(rhs)) {}

  std::string prettify(void) const override {
    return std::format("(unary {} {})", op.value, rhs->prettify());
  }
};

class BinaryExpression : public ExpressionNode {
public:
  Expression lhs;
  const Token &op;
  Expression rhs;

  explicit BinaryExpression(Expression&& lhs, const Token &op, Expression&& rhs)
      : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}

  std::string prettify(void) const override {
    return std::format("(binary {} {} {})", lhs->prettify(), op.value,
                       rhs->prettify());
  }
};

class GroupingExpression : public ExpressionNode {
public:
  Expression expr;

  explicit GroupingExpression(Expression&& expr) : expr(std::move(expr)) {}

  std::string prettify(void) const override {
    return std::format("(grouping {})", expr->prettify());
  }
};

class InlineIf : public ExpressionNode {
public:
  Expression condition;
  Expression consequent;
  Expression alternate;

  explicit InlineIf(Expression&& condition, Expression&& consequent,
                    Expression&& alternate)
      : condition(std::move(condition)), consequent(std::move(consequent)),
        alternate(std::move(alternate)) {}

  std::string prettify(void) const override {
    return std::format("(if {} then {} else {})", condition->prettify(),
                       consequent->prettify(), alternate->prettify());
  }
};

class AsExpression : public ExpressionNode {
public:
  Expression expr;
  Type type;

  explicit AsExpression(Expression&& expr, Type &&type): expr(std::move(expr)), type(std::move(type)) {}

  std::string prettify(void) const override {
    return std::format("(as {} {})", expr->prettify(), type->prettify());
  }
};

class IdentifierExpression : public ExpressionNode {
public:
  const Token &value;

  explicit IdentifierExpression(const Token &v) : value(v) {}

  std::string prettify(void) const override {
    switch (value.type) {
    case TokenType::Identifier:
      return std::format("(ident {})", value.value);
    case TokenType::SpicialIdentifier:
      return std::format("(spicial_ident {})", value.value);
    default:
      std::exit(70);
    }
  }
};

} // namespace AST

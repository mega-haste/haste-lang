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
  virtual const std::string prettify(void) const = 0;
};

using Expression = std::unique_ptr<ExpressionNode>;

class LiteralExpression : public ExpressionNode {
public:
  const Token &value;

  explicit LiteralExpression(const Token &v) : value(v) {}

  const std::string prettify(void) const override {
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

  const std::string prettify(void) const override {
    return value ? "(bool true)" : "(bool false)";
  }
};

class UnaryExpression : public ExpressionNode {
public:
  const Token &op;
  Expression rhs;

  explicit UnaryExpression(const Token &op, Expression&& rhs)
      : op(op), rhs(std::move(rhs)) {}

  const std::string prettify(void) const override {
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

  const std::string prettify(void) const override {
    return std::format("(binary {} {} {})", lhs->prettify(), op.value,
                       rhs->prettify());
  }
};

class GroupingExpression : public ExpressionNode {
public:
  Expression expr;

  explicit GroupingExpression(Expression&& expr) : expr(std::move(expr)) {}

  const std::string prettify(void) const override {
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

  const std::string prettify(void) const override {
    return std::format("(if {} then {} else {})", condition->prettify(),
                       consequent->prettify(), alternate->prettify());
  }
};

class AsExpression : public ExpressionNode {
public:
  Expression expr;
  Type type;

  explicit AsExpression(Expression&& expr, Type &&type): expr(std::move(expr)), type(std::move(type)) {}

  const std::string prettify(void) const override {
    return std::format("(as {} {})", expr->prettify(), type->prettify());
  }
};

class IdentifierExpression : public ExpressionNode {
public:
  const Token &value;

  explicit IdentifierExpression(const Token &v) : value(v) {}

  const std::string prettify(void) const override {
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

class CallExpression : public ExpressionNode {
public:
  Expression callee;
  const Token &paren;
  std::vector<Expression> arguments;

  explicit CallExpression(Expression &&callee, const Token &paren, std::vector<Expression> arguments)
    : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}

  const std::string prettify(void) const override {
    std::string arg_str;
    for (auto &argument : arguments) {
      arg_str.append(argument->prettify());
      arg_str.append(", ");
    }

    return std::format("(call {}({}))", callee->prettify(), arg_str);
  }
};

class MemberAccessExpression : public ExpressionNode {
public:
  Expression group;
  const Token &dot;
  const Token &member;

  explicit MemberAccessExpression(Expression &&group, const Token &dot, const Token &member)
    : group(std::move(group)), dot(dot), member(std::move(member)) {}

  const std::string prettify(void) const override {
    return std::format("(access {} {})", group->prettify(), member.value);
  }
};

} // namespace AST

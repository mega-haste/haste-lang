#pragma once

#include "Analysis/Context.hpp"
#include "Expressions.hpp"
#include "tokens.hpp"
#include <cstdio>
#include <format>
#include <memory>
#include <string>
#include <vector>

namespace AST {
std::string repeat_char(char c, int times);

class StatementNode;

using Statement = std::unique_ptr<StatementNode>;

class StatementNode {
public:
  virtual const std::string prettify(const int depth = 0) const = 0;
  virtual void analyse(Analysis::Context &ctx) const {};

  virtual ~StatementNode() = default;
};

class BlockStatement : public StatementNode {
public:
  std::vector<Statement> statements;

  explicit BlockStatement(std::vector<Statement> statements)
      : statements(std::move(statements)) {}

  const std::string prettify(const int depth = 0) const override {
    std::string body_str;
    for (auto &b : statements) {
      body_str.push_back('\n');
      body_str.append(b->prettify(depth + 4));
    }
    std::string ssssss = repeat_char(' ', depth);
    return std::format("{}(block {}{}\n{}{})", ssssss, '{', body_str, ssssss,
                       '}');
  }
};

class ExpressionStatement : public StatementNode {
public:
  Expression expr;

  explicit ExpressionStatement(Expression expr) : expr(std::move(expr)) {}

  const std::string prettify(const int depth = 0) const override {

    return std::format("{}(expr {})", repeat_char(' ', depth),
                       expr->prettify());
  }
};

class FunctionDef : public StatementNode {
public:
  const Token &identifier;
  std::vector<TypedIdentifier> arguments;
  Type return_type;
  Statement body;

  FunctionDef(const Token &identifier, std::vector<TypedIdentifier> arguments,
              Type &&return_type, Statement &&body)
      : identifier(identifier), arguments(std::move(arguments)),
        return_type(std::move(return_type)), body(std::move(body)) {}

  const std::string prettify(const int depth = 0) const override {
    std::string args;
    std::string body_str;
    for (auto &arg : arguments) {
      args.append(", ");
      args.append(arg.prettify());
    }
    std::string ssss = repeat_char(' ', depth);

    return std::format("{}(func_dec name:{} args:({}) ret_type:{} body:({}))",
                       ssss, identifier.value, args, return_type->prettify(),
                       body->prettify(depth + 4));
  }
};

class LetDef : public StatementNode {
public:
  const Token &ident;
  std::optional<Expression> value;
  std::optional<Type> type;
  bool mut;

  explicit LetDef(const Token &ident, std::optional<Expression> &&value,
                  std::optional<Type> &&type, bool mut)
      : ident(ident), value(std::move(value)), type(std::move(type)), mut(mut) {
  }

  const std::string prettify(const int depth = 0) const override {
    return std::format(
        "{}(let_dec name:{} mut:{} type:{} value:{})", repeat_char(' ', depth),
        ident.value, mut, type.has_value() ? type.value()->prettify() : "unset",
        value.has_value() ? value.value()->prettify() : "undefined");
  }
};

class IfStatement : public StatementNode {
public:
  Expression condition;
  Statement then;
  std::optional<Statement> otherwise;

  explicit IfStatement(Expression &&condition, Statement &&then,
                       std::optional<Statement> &&otherwise)
      : condition(std::move(condition)), then(std::move(then)),
        otherwise(std::move(otherwise)) {}

  const std::string prettify(const int depth = 0) const override {
    return std::format(
        "{}(if ({}) {} else {})", repeat_char(' ', depth),
        condition->prettify(), then->prettify(depth),
        otherwise.has_value() ? otherwise.value()->prettify(depth) : "NONE");
  }
};

class ReturnStatement : public StatementNode {
public:
  std::optional<Expression> expr;

  explicit ReturnStatement(std::optional<Expression> &&expr)
      : expr(std::move(expr)) {}

  const std::string prettify(const int depth = 0) const override {
    return std::format("{}(return {})", repeat_char(' ', depth),
                       expr.has_value() ? expr.value()->prettify() : "NONE");
  }
};

} // namespace AST

#pragma once

#include "Analysis/Context.hpp"
#include "Analysis/Symbol.hpp"
#include "Expressions.hpp"
#include "macros.hpp"
#include "tokens.hpp"
#include <cmath>
#include <cstdio>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace AST {
std::string repeat_char(char c, int times);

class StatementNode;

using Statement = std::unique_ptr<StatementNode>;
using Analysis::NativeType;
using Analysis::SymbolHandler;
using Analysis::SymbolType;

class StatementNode {
public:
  virtual const std::string prettify(const int depth = 0) const = 0;
  virtual void analyse(Analysis::Context &ctx) const {};

  virtual ~StatementNode() = default;
};

class NoneStmt : public StatementNode {
  virtual const std::string prettify(const int depth = 0) const {
    return "None";
  }
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

  void analyse(Analysis::Context &ctx) const override {
    ctx.symbol_table.scope_begin();
    for (auto &statement : statements) {
      statement->analyse(ctx);
    }
    ctx.symbol_table.scope_end();
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

  void analyse(Analysis::Context &ctx) const override { expr->analyse(ctx); }
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

  void analyse(Analysis::Context &ctx) const override {
    ctx.declare(identifier.value);
    Analysis::SymbolFunctionType type = Analysis::SymbolFunctionType(
        std::make_unique<SymbolType>(return_type->get_type()));
    for (auto &argument : arguments) {
      type.args.push_back(Analysis::SymbolFunctionType::Arg(
          argument.identifier.value,
          std::make_shared<SymbolType>(argument.type->get_type())));
    }
    ctx.define(identifier.value, std::move(type), false);

    ctx.symbol_table.scope_begin();
    type.define_args(ctx);
    body->analyse(ctx);
    ctx.symbol_table.scope_end();
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

  void analyse(Analysis::Context &ctx) const override {
    ctx.declare(ident.value);
    SymbolType expected_type = type.has_value() ? type.value()->get_type()
                                                : Analysis::NativeType::Auto;
    SymbolType value_type = value.has_value() ? value.value()->get_type(ctx)
                                              : NativeType::Undefined;
    if (Analysis::is_auto(expected_type)) {
      expected_type = std::move(value_type);

      if (Analysis::is_auto(expected_type) ||
          Analysis::is_undefined(expected_type)) {
        std::cerr
            << "[" << ident.line << ":" << ident.column << "] "
            << "You need either set a known-type value to the variable, or "
               "explicitly set the type of the variable.\n";
        return;
      }
      ctx.define(ident.value, std::move(expected_type), mut);
      return;
    }
    if (Analysis::is_undefined(value_type)) {
      std::cerr << "[" << ident.line << ":" << ident.column << "] "
                << "You need either set a known-type value to the variable, or "
                   "explicitly set the type of the variable.\n";
      return;
    }
    if (!Analysis::match(expected_type, value_type)) {
      std::cerr << "[" << ident.line << ":" << ident.column << "] "
                << "Your variable types doesn't match the value type. try "
                   "implicit type inference or use `auto` instead.\n";
      return;
    }
    LOG(Analysis::prettify(expected_type)
        << " " << Analysis::prettify(value_type));
    ctx.define(ident.value, std::move(expected_type), mut);
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

  void analyse(Analysis::Context &ctx) const override {
    if (!Analysis::is_bool(condition->get_type(ctx))) {
      std::cerr << "[" << "??" << ":" << "??" << "] "
                << "Expected it to be a boolean.\n";
      return;
    }
    then->analyse(ctx);
    if (otherwise.has_value())
      otherwise.value()->analyse(ctx);
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

  void analyse(Analysis::Context &ctx) const override {
    if (!ctx.is_in_function()) {
      std::cerr << "[" << "??" << ":" << "??" << "] "
                << "`return` should be ALWAYS in a function.\n";
      return;
    }
  }
};

} // namespace AST

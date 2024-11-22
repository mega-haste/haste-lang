#include "AST/Expressions.hpp"
#include "AST/Statments.hpp"
#include "Analysis/Context.hpp"
#include "Analysis/Symbol.hpp"
#include "common.hpp"
#include "tokens.hpp"
#include <cmath>
#include <cstdio>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace AST {

using Analysis::Context;

std::string repeat_char(char c, int times) {
  std::string s;
  for (int i = 0; i < times; i++) {
    s.push_back(c);
  }

  return s;
}

void StatementNode::analyse(Analysis::Context &ctx) const { UNUSED(ctx); };

std::string NoneStmt::prettify(const int depth) const {
  UNUSED(depth);
  return "None";
}

BlockStatement::BlockStatement(std::vector<Statement> statements)
    : statements(std::move(statements)) {}
std::string BlockStatement::prettify(const int depth) const {
  std::string body_str;
  for (auto &b : statements) {
    body_str.push_back('\n');
    body_str.append(b->prettify(depth + 4));
  }
  std::string ssssss = repeat_char(' ', depth);
  return std::format("{}(block {}{}\n{}{})", ssssss, '{', body_str, ssssss,
                     '}');
}
void BlockStatement::analyse(Analysis::Context &ctx) const {
  ctx.scope_begin();
  for (auto &statement : statements) {
    statement->analyse(ctx);
  }
  ctx.scope_end();
}

ExpressionStatement::ExpressionStatement(Expression expr)
    : expr(std::move(expr)) {}
std::string ExpressionStatement::prettify(const int depth) const {
  return std::format("{}(expr {})", repeat_char(' ', depth), expr->prettify());
}
void ExpressionStatement::analyse(Analysis::Context &ctx) const {
  expr->analyse(ctx);
}

FunctionDef::FunctionDef(const Token &identifier,
                         std::vector<TypedIdentifier> arguments,
                         Type &&return_type, Statement &&body)
    : identifier(identifier), arguments(std::move(arguments)),
      return_type(std::move(return_type)), body(std::move(body)) {}
std::string FunctionDef::prettify(const int depth) const {
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
void FunctionDef::analyse(Analysis::Context &ctx) const {
  ctx.declare(identifier);
  Analysis::SymbolFunctionType type = Analysis::SymbolFunctionType(
      std::make_unique<SymbolType>(return_type->get_type()));
  for (auto &argument : arguments) {
    type.args.push_back(Analysis::SymbolFunctionType::Arg(
        argument.identifier,
        std::make_shared<SymbolType>(argument.type->get_type())));
  }
  ctx.define(identifier, type, false);

  ctx.scope_begin();

  type.define_args(ctx);

  ctx.current_func = Context::FuncType::Function;
  ctx.function_stack.push_back(identifier.value);

  body->analyse(ctx);

  ctx.current_func = Context::FuncType::None;
  ctx.function_stack.pop_back();

  ctx.scope_end();
}

LetDef::LetDef(const Token &ident, std::optional<Expression> &&value,
               std::optional<Type> &&type, bool mut)
    : ident(ident), value(std::move(value)), type(std::move(type)), mut(mut) {}
std::string LetDef::prettify(const int depth) const {
  return std::format(
      "{}(let_dec name:{} mut:{} type:{} value:{})", repeat_char(' ', depth),
      ident.value, mut, type.has_value() ? type.value()->prettify() : "unset",
      value.has_value() ? value.value()->prettify() : "undefined");
}
void LetDef::analyse(Analysis::Context &ctx) const {
  ctx.declare(ident);

  SymbolType expected_type =
      type.has_value() ? type.value()->get_type() : Analysis::NativeType::Auto;
  SymbolType value_type =
      value.has_value() ? value.value()->get_type(ctx) : NativeType::Undefined;

  if (Analysis::is_auto(expected_type)) {
    expected_type = std::move(value_type);

    if (Analysis::is_auto(expected_type) ||
        Analysis::is_undefined(expected_type)) {
      ctx.report_error(ident, "Unknown type",
                       "You need either set a known-type value to the "
                       "variable, or explicitly set the type of the variable.");
      return;
    }
    if (value.has_value())
      value.value()->analyse(ctx);
    ctx.define(ident, std::move(expected_type), mut);
    return;
  }

  if (Analysis::is_undefined(value_type)) {
    ctx.report_error(ident, "Unknown type",
                     "You need either set a known-type value to the variable, "
                     "or explicitly set the type of the variable.");
    return;
  }

  if (!Analysis::match(expected_type, value_type)) {
    ctx.report_error(ident, "Unknown type",
                     "Your variable types doesn't match the value type. Try "
                     "implicit type inference or use `auto` instead.");
    return;
  }

  if (value.has_value())
    value.value()->analyse(ctx);
  ctx.define(ident, std::move(expected_type), mut);
}

IfStatement::IfStatement(Expression &&condition, Statement &&then,
                         std::optional<Statement> &&otherwise)
    : condition(std::move(condition)), then(std::move(then)),
      otherwise(std::move(otherwise)) {}
std::string IfStatement::prettify(const int depth) const {
  return std::format("{}(if ({}) {} else {})", repeat_char(' ', depth),
                     condition->prettify(), then->prettify(depth),
                     otherwise.has_value() ? otherwise.value()->prettify(depth)
                                           : "NONE");
}
void IfStatement::analyse(Analysis::Context &ctx) const {
  if (!Analysis::is_bool(condition->get_type(ctx))) {
    std::cerr << "[" << "??" << ":" << "??" << "] "
              << "Expected it to be a boolean.\n";
    return;
  }
  then->analyse(ctx);
  if (otherwise.has_value())
    otherwise.value()->analyse(ctx);
}

ReturnStatement::ReturnStatement(std::optional<Expression> &&expr)
    : expr(std::move(expr)) {}
std::string ReturnStatement::prettify(const int depth) const {
  return std::format("{}(return {})", repeat_char(' ', depth),
                     expr.has_value() ? expr.value()->prettify() : "NONE");
}
void ReturnStatement::analyse(Analysis::Context &ctx) const {
  if (!ctx.is_in_function()) {
    std::cerr << "[" << "??" << ":" << "??" << "] "
              << "`return` should be ALWAYS in a function.\n";
    return;
  }
}

} // namespace AST

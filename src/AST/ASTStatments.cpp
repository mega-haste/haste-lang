#include "AST/Expressions.hpp"
#include "AST/Statments.hpp"
#include "AST/TypeNode.hpp"
#include "Analysis/Context.hpp"
#include "Analysis/Types.hpp"
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

void StatementNode::analyse(Context &ctx) const { UNUSED(ctx); };

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
                         TypeNode::Handler &&return_type, Statement &&body)
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
  UNUSED(ctx);
  UNIMPLEMENTED("FunctionDef::analyse");
  // ctx.declare(identifier);
  // Analysis::CallableType::Handler type =
  //     Analysis::CallableType::make(return_type->get_type());
  // for (auto &argument : arguments) {
  //   type->arguments.push_back(Analysis::CallableType::Arg(
  //       argument.identifier, argument.type->get_type()));
  // }
  // ctx.define(
  //   identifier,
  //   Type::make(type),
  //   false
  // );
  //
  // ctx.scope_begin();
  //
  // type.define_args(ctx);
  //
  // ctx.current_func = Context::FuncType::Function;
  // ctx.function_stack.push_back(identifier.value);
  //
  // body->analyse(ctx);
  //
  // ctx.current_func = Context::FuncType::None;
  // ctx.function_stack.pop_back();
  //
  // ctx.scope_end();
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
  UNUSED(ctx);
  UNIMPLEMENTED("IfStatement::analyse");
  // condition->analyse(ctx);
  // auto condition_type = condition->get_type(ctx);
  // if (!condition_type->is_bool()) {
  //   ctx.report_error(
  //       condition->start, "Mismatch types",
  //       std::format("Expected `bool` got `{}`.",
  //       condition_type->prettify()));
  // }
  // then->analyse(ctx);
  // if (otherwise.has_value())
  //   otherwise.value()->analyse(ctx);
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

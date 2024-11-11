#pragma once

#include <cstdio>
#include <memory>
#include <string>
#include "Expressions.hpp"
#include "sigma.hpp"

namespace AST {

class StatementNode;

using Statement = std::unique_ptr<StatementNode>;

class StatementNode {
public:
  virtual std::string prettify(void) const = 0;
};

class ExpressionStatement : public StatementNode {
public:
  Expression expr;
  
  explicit ExpressionStatement(Expression expr) : expr(std::move(expr)) {}

  std::string prettify(void) const override {
    return std::format("(expr {})", expr->prettify());
  }
};

class FunctionDef : public StatementNode {
public:
  const Token &identifier;
  std::vector<TypedIdentifier> arguments;
  Type return_type;
  std::vector<Statement> body;

  explicit FunctionDef(const Token &identifier, std::vector<TypedIdentifier> arguments, Type return_type, std::vector<Statement> &&body)
    : identifier(identifier), arguments(std::move(arguments)), return_type(std::move(return_type)), body(std::move(body)) {}

  std::string prettify(void) const override {
    std::string args;
    std::string body_str;
    for (auto &arg : arguments) {
      args.append(", ");
      args.append(arg.prettify());
    }

    for (auto &b : body) {
      body_str.push_back('\n');
      body_str.append(b->prettify());
    }

    return std::format("(func_dec {}({}) -> {} ({}))", identifier.value, args, return_type->prettify(), body_str);
  }
};

} // namespace AST

#pragma once

#include "Analysis/Context.hpp"
#include "Analysis/Symbol.hpp"
#include "Expressions.hpp"
#include "tokens.hpp"
#include <cmath>
#include <cstdio>
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
  virtual std::string prettify(const int depth = 0) const = 0;
  virtual void analyse(Analysis::Context &ctx) const;

  virtual ~StatementNode() = default;
};

class NoneStmt : public StatementNode {
  std::string prettify(const int depth = 0) const override;
};

class BlockStatement : public StatementNode {
public:
  std::vector<Statement> statements;

  explicit BlockStatement(std::vector<Statement> statements);
  std::string prettify(const int depth = 0) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class ExpressionStatement : public StatementNode {
public:
  Expression expr;

  explicit ExpressionStatement(Expression expr);
  std::string prettify(const int depth = 0) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class FunctionDef : public StatementNode {
public:
  const Token &identifier;
  std::vector<TypedIdentifier> arguments;
  Type return_type;
  Statement body;

  explicit FunctionDef(const Token &identifier,
                       std::vector<TypedIdentifier> arguments,
                       Type &&return_type, Statement &&body);
  std::string prettify(const int depth = 0) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class LetDef : public StatementNode {
public:
  const Token &ident;
  std::optional<Expression> value;
  std::optional<Type> type;
  bool mut;

  explicit LetDef(const Token &ident, std::optional<Expression> &&value,
                  std::optional<Type> &&type, bool mut);
  std::string prettify(const int depth = 0) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class IfStatement : public StatementNode {
public:
  Expression condition;
  Statement then;
  std::optional<Statement> otherwise;

  explicit IfStatement(Expression &&condition, Statement &&then,
                       std::optional<Statement> &&otherwise);
  std::string prettify(const int depth = 0) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class ReturnStatement : public StatementNode {
public:
  std::optional<Expression> expr;

  explicit ReturnStatement(std::optional<Expression> &&expr);
  std::string prettify(const int depth = 0) const override;
  void analyse(Analysis::Context &ctx) const override;
};

} // namespace AST

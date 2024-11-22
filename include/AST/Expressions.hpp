#pragma once

#include "Analysis/Context.hpp"
#include "Analysis/Symbol.hpp"
#include "TypeNode.hpp"
#include "tokens.hpp"

namespace AST {

using Analysis::NativeType;
using Analysis::SymbolHandler;
using Analysis::SymbolType;

class ExpressionNode {
public:
  Token start = Token();
  Token end = Token();

  virtual std::string prettify(void) const = 0;
  virtual SymbolType get_type(Analysis::Context &ctx) const = 0;
  virtual void analyse(Analysis::Context &ctx) const;

  virtual ~ExpressionNode() = default;
};

using Expression = std::unique_ptr<ExpressionNode>;

class ExpressionNone : public ExpressionNode {
public:
  std::string prettify() const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
};

class LiteralExpression : public ExpressionNode {
public:
  const Token &value;

  explicit LiteralExpression(const Token &v);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
};

class BooleanExpression : public ExpressionNode {
public:
  bool value;

  explicit BooleanExpression(bool v);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
};

class UnaryExpression : public ExpressionNode {
public:
  const Token &op;
  Expression rhs;

  explicit UnaryExpression(const Token &op, Expression &&rhs);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class BinaryExpression : public ExpressionNode {
public:
  Expression lhs;
  const Token &op;
  Expression rhs;

  explicit BinaryExpression(Expression &&lhs, const Token &op,
                            Expression &&rhs);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class GroupingExpression : public ExpressionNode {
public:
  Expression expr;

  explicit GroupingExpression(Expression &&expr);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class InlineIf : public ExpressionNode {
public:
  Expression condition;
  Expression consequent;
  Expression alternate;

  explicit InlineIf(Expression &&condition, Expression &&consequent,
                    Expression &&alternate);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class AsExpression : public ExpressionNode {
public:
  Expression expr;
  Type type;

  explicit AsExpression(Expression &&expr, Type &&type);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class IdentifierExpression : public ExpressionNode {
public:
  const Token &value;

  explicit IdentifierExpression(const Token &v);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class CallExpression : public ExpressionNode {
public:
  Expression callee;
  const Token &paren;
  std::vector<Expression> arguments;

  explicit CallExpression(Expression &&callee, const Token &paren,
                          std::vector<Expression> arguments);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
  void analyse(Analysis::Context &ctx) const override;
};

class MemberAccessExpression : public ExpressionNode {
public:
  Expression group;
  const Token &dot;
  const Token &member;

  explicit MemberAccessExpression(Expression &&group, const Token &dot,
                                  const Token &member);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
};

class SubscriptingExpression : public ExpressionNode {
public:
  Expression item;
  const Token &open_brackets;
  const Token &close_brackets;
  Expression index;

  explicit SubscriptingExpression(Expression &&item, const Token &open_brackets,
                                  const Token &close_brackets,
                                  Expression &&index);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
};

class TupleExpression : public ExpressionNode {
public:
  std::vector<Expression> elements;
  const Token &closing_parentheses;

  explicit TupleExpression(std::vector<Expression> &&elements,
                           const Token &closing_parentheses);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
};

class ScopeResolutionExpression : public ExpressionNode {
public:
  Expression group;
  const Token &colon;
  const Token &member;

  explicit ScopeResolutionExpression(Expression &&group, const Token &colon,
                                     const Token &member);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
};

class AssignExpression : public ExpressionNode {
public:
  Expression lhs;
  const Token &eq;
  Expression value;

  explicit AssignExpression(Expression &&lhs, const Token &eq,
                            Expression &&value);
  std::string prettify(void) const override;
  SymbolType get_type(Analysis::Context &ctx) const override;
  void analyse(Analysis::Context &ctx) const override;
};

} // namespace AST

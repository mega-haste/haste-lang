#pragma once

#include "Analysis/Context.hpp"
#include "Analysis/Symbol.hpp"
#include "TypeNode.hpp"
#include "macros.hpp"
#include "tokens.hpp"
#include <cmath>
#include <cstdlib>
#include <format>
#include <memory>
#include <string>

namespace AST {

using Analysis::NativeType;
using Analysis::SymbolHandler;
using Analysis::SymbolType;

class ExpressionNode {
public:
  virtual const std::string prettify(void) const = 0;
  virtual SymbolType get_type(Analysis::Context &ctx) const = 0;
  virtual const void analyse(Analysis::Context &ctx) const {};
};

using Expression = std::unique_ptr<ExpressionNode>;

class ExpressionNone : public ExpressionNode {
public:
  const std::string prettify() const override { return "(NONE)"; }
  SymbolType get_type(Analysis::Context &ctx) const override {
    return Analysis::NativeType::Unknown;
  }
};

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
  SymbolType get_type(Analysis::Context &ctx) const override {
    switch (value.type) {
    case TokenType::CharLit:
      return NativeType::Char;
    case TokenType::StringLit:
      return NativeType::String;
    case TokenType::IntLit:
      return NativeType::Int;
    case TokenType::FloatLit:
      return NativeType::Float;
    default:
      return NativeType::Unknown;
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
  SymbolType get_type(Analysis::Context &ctx) const override {
    return NativeType::Bool;
  }
};

class UnaryExpression : public ExpressionNode {
public:
  const Token &op;
  Expression rhs;

  explicit UnaryExpression(const Token &op, Expression &&rhs)
      : op(op), rhs(std::move(rhs)) {}

  const std::string prettify(void) const override {
    return std::format("(unary {} {})", op.value, rhs->prettify());
  }

  SymbolType get_type(Analysis::Context &ctx) const override {
    return Analysis::do_unary(op, rhs->get_type(ctx));
  }
};

class BinaryExpression : public ExpressionNode {
public:
  Expression lhs;
  const Token &op;
  Expression rhs;

  explicit BinaryExpression(Expression &&lhs, const Token &op, Expression &&rhs)
      : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}

  const std::string prettify(void) const override {
    return std::format("(binary {} {} {})", lhs->prettify(), op.value,
                       rhs->prettify());
  }

  SymbolType get_type(Analysis::Context &ctx) const override {
    return Analysis::do_binary(lhs->get_type(ctx), op, rhs->get_type(ctx));
  }
};

class GroupingExpression : public ExpressionNode {
public:
  Expression expr;

  explicit GroupingExpression(Expression &&expr) : expr(std::move(expr)) {}

  const std::string prettify(void) const override {
    return std::format("(grouping {})", expr->prettify());
  }

  SymbolType get_type(Analysis::Context &ctx) const override {
    return expr->get_type(ctx);
  }
};

class InlineIf : public ExpressionNode {
public:
  Expression condition;
  Expression consequent;
  Expression alternate;

  explicit InlineIf(Expression &&condition, Expression &&consequent,
                    Expression &&alternate)
      : condition(std::move(condition)), consequent(std::move(consequent)),
        alternate(std::move(alternate)) {}

  const std::string prettify(void) const override {
    return std::format("(if {} then {} else {})", condition->prettify(),
                       consequent->prettify(), alternate->prettify());
  }

  SymbolType get_type(Analysis::Context &ctx) const override {
    return consequent->get_type(ctx);
  }
};

class AsExpression : public ExpressionNode {
public:
  Expression expr;
  Type type;

  explicit AsExpression(Expression &&expr, Type &&type)
      : expr(std::move(expr)), type(std::move(type)) {}

  const std::string prettify(void) const override {
    return std::format("(as {} {})", expr->prettify(), type->prettify());
  }

  SymbolType get_type(Analysis::Context &ctx) const override {
    return type->get_type();
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

  SymbolType get_type(Analysis::Context &ctx) const override {
    if (ctx.is_defined(value.value))
      return *ctx.symbol_table.local_first_look_up(value.value)->type;
    return NativeType::Unknown;
  }
};

class CallExpression : public ExpressionNode {
public:
  Expression callee;
  const Token &paren;
  std::vector<Expression> arguments;

  explicit CallExpression(Expression &&callee, const Token &paren,
                          std::vector<Expression> arguments)
      : callee(std::move(callee)), paren(paren),
        arguments(std::move(arguments)) {}

  const std::string prettify(void) const override {
    std::string arg_str;
    for (auto &argument : arguments) {
      arg_str.append(argument->prettify());
      arg_str.append(", ");
    }
    if (arg_str.size() > 2) {
      arg_str.pop_back();
      arg_str.pop_back();
    }

    return std::format("(call {}({}))", callee->prettify(), arg_str);
  }
  SymbolType get_type(Analysis::Context &ctx) const override {
    UNIMPLEMENTED("CallExpression");
  }
};

class MemberAccessExpression : public ExpressionNode {
public:
  Expression group;
  const Token &dot;
  const Token &member;

  explicit MemberAccessExpression(Expression &&group, const Token &dot,
                                  const Token &member)
      : group(std::move(group)), dot(dot), member(std::move(member)) {}

  const std::string prettify(void) const override {
    return std::format("(access {} {})", group->prettify(), member.value);
  }

  SymbolType get_type(Analysis::Context &ctx) const override {
    UNIMPLEMENTED("MemberAccessExpression");
  }
};

class SubscriptingExpression : public ExpressionNode {
public:
  Expression item;
  const Token &open_brackets;
  const Token &close_brackets;
  Expression index;

  explicit SubscriptingExpression(Expression &&item, const Token &open_brackets,
                                  const Token &close_brackets,
                                  Expression &&index)
      : item(std::move(item)), open_brackets(open_brackets),
        close_brackets(close_brackets), index(std::move(index)) {}

  const std::string prettify(void) const override {
    return std::format("(subscript {} {})", item->prettify(),
                       index->prettify());
  }
  SymbolType get_type(Analysis::Context &ctx) const override {
    UNIMPLEMENTED("SubscriptingExpression");
  }
};

class TupleExpression : public ExpressionNode {
public:
  std::vector<Expression> elements;
  const Token &closing_parentheses;

  explicit TupleExpression(std::vector<Expression> &&elements,
                           const Token &closing_parentheses)
      : elements(std::move(elements)),
        closing_parentheses(closing_parentheses) {}

  const std::string prettify(void) const override {
    std::string ele_str;

    for (auto &e : elements) {
      ele_str.append(e->prettify());
      ele_str.append(", ");
    }
    if (ele_str.size() > 2) {
      ele_str.pop_back();
      ele_str.pop_back();
    }

    return std::format("(tuple ({}))", ele_str);
  }
  SymbolType get_type(Analysis::Context &ctx) const override {
    UNIMPLEMENTED("Tuple");
  }
};

class ScopeResolutionExpression : public ExpressionNode {
public:
  Expression group;
  const Token &colon;
  const Token &member;

  explicit ScopeResolutionExpression(Expression &&group, const Token &colon,
                                     const Token &member)
      : group(std::move(group)), colon(colon), member(member) {}

  const std::string prettify(void) const override {
    return std::format("(scope_resolution {} {})", group->prettify(),
                       member.value);
  }
  SymbolType get_type(Analysis::Context &ctx) const override {
    UNIMPLEMENTED("ScopeResolutionExpression");
  }
};

class AssignExpression : public ExpressionNode {
public:
  Expression lhs;
  const Token &eq;
  Expression value;

  explicit AssignExpression(Expression &&lhs, const Token &eq,
                            Expression &&value)
      : lhs(std::move(lhs)), eq(eq), value(std::move(value)) {}

  const std::string prettify(void) const override {
    return std::format("(assign {} {} {})", lhs->prettify(), eq.value,
                       value->prettify());
  }
  SymbolType get_type(Analysis::Context &ctx) const override {
    return value->get_type(ctx);
  }
};

} // namespace AST

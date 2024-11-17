#include "AST/Expressions.hpp"
#include "Analysis/Symbol.hpp"
#include "macros.hpp"
#include "tokens.hpp"
#include <cmath>
#include <cstdlib>
#include <format>
#include <iostream>
#include <memory>
#include <string>

namespace AST {
using Analysis::NativeType;
using Analysis::SymbolHandler;
using Analysis::SymbolType;

const void ExpressionNode::analyse(Analysis::Context &ctx) const {};

const std::string ExpressionNone::prettify() const { return "(NONE)"; }
SymbolType ExpressionNone::get_type(Analysis::Context &ctx) const {
  return Analysis::NativeType::Unknown;
}

LiteralExpression::LiteralExpression(const Token &v) : value(v) {}
const std::string LiteralExpression::prettify(void) const {
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
SymbolType LiteralExpression::get_type(Analysis::Context &ctx) const {
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

BooleanExpression::BooleanExpression(bool v) : value(v) {}
const std::string BooleanExpression::prettify(void) const {
  return value ? "(bool true)" : "(bool false)";
}
SymbolType BooleanExpression::get_type(Analysis::Context &ctx) const {
  return NativeType::Bool;
}

UnaryExpression::UnaryExpression(const Token &op, Expression &&rhs)
    : op(op), rhs(std::move(rhs)) {}
const std::string UnaryExpression::prettify(void) const {
  return std::format("(unary {} {})", op.value, rhs->prettify());
}
SymbolType UnaryExpression::get_type(Analysis::Context &ctx) const {
  return Analysis::do_unary(op, rhs->get_type(ctx));
}

BinaryExpression::BinaryExpression(Expression &&lhs, const Token &op,
                                   Expression &&rhs)
    : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}
const std::string BinaryExpression::prettify(void) const {
  return std::format("(binary {} {} {})", lhs->prettify(), op.value,
                     rhs->prettify());
}
SymbolType BinaryExpression::get_type(Analysis::Context &ctx) const {
  return Analysis::do_binary(lhs->get_type(ctx), op, rhs->get_type(ctx));
}

GroupingExpression::GroupingExpression(Expression &&expr)
    : expr(std::move(expr)) {}
const std::string GroupingExpression::prettify(void) const {
  return std::format("(grouping {})", expr->prettify());
}
SymbolType GroupingExpression::get_type(Analysis::Context &ctx) const {
  return expr->get_type(ctx);
}

InlineIf::InlineIf(Expression &&condition, Expression &&consequent,
                   Expression &&alternate)
    : condition(std::move(condition)), consequent(std::move(consequent)),
      alternate(std::move(alternate)) {}
const std::string InlineIf::prettify(void) const {
  return std::format("(if {} then {} else {})", condition->prettify(),
                     consequent->prettify(), alternate->prettify());
}
SymbolType InlineIf::get_type(Analysis::Context &ctx) const {
  return consequent->get_type(ctx);
}

AsExpression::AsExpression(Expression &&expr, Type &&type)
    : expr(std::move(expr)), type(std::move(type)) {}
const std::string AsExpression::prettify(void) const {
  return std::format("(as {} {})", expr->prettify(), type->prettify());
}
SymbolType AsExpression::get_type(Analysis::Context &ctx) const {
  return type->get_type();
}

IdentifierExpression::IdentifierExpression(const Token &v) : value(v) {}
const std::string IdentifierExpression::prettify(void) const {
  switch (value.type) {
  case TokenType::Identifier:
    return std::format("(ident {})", value.value);
  case TokenType::SpicialIdentifier:
    return std::format("(spicial_ident {})", value.value);
  default:
    std::exit(70);
  }
}
SymbolType IdentifierExpression::get_type(Analysis::Context &ctx) const {
  if (ctx.is_defined(value.value))
    return *ctx.symbol_table.local_first_look_up(value.value).value().type;
  return NativeType::Unknown;
}

CallExpression::CallExpression(Expression &&callee, const Token &paren,
                               std::vector<Expression> arguments)
    : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {
}
const std::string CallExpression::prettify(void) const {
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
SymbolType CallExpression::get_type(Analysis::Context &ctx) const {
  UNIMPLEMENTED("CallExpression");
}

MemberAccessExpression::MemberAccessExpression(Expression &&group,
                                               const Token &dot,
                                               const Token &member)
    : group(std::move(group)), dot(dot), member(std::move(member)) {}
const std::string MemberAccessExpression::prettify(void) const {
  return std::format("(access {} {})", group->prettify(), member.value);
}
SymbolType MemberAccessExpression::get_type(Analysis::Context &ctx) const {
  UNIMPLEMENTED("MemberAccessExpression");
}

SubscriptingExpression::SubscriptingExpression(Expression &&item,
                                               const Token &open_brackets,
                                               const Token &close_brackets,
                                               Expression &&index)
    : item(std::move(item)), open_brackets(open_brackets),
      close_brackets(close_brackets), index(std::move(index)) {}
const std::string SubscriptingExpression::prettify(void) const {
  return std::format("(subscript {} {})", item->prettify(), index->prettify());
}
SymbolType SubscriptingExpression::get_type(Analysis::Context &ctx) const {
  UNIMPLEMENTED("SubscriptingExpression");
}

TupleExpression::TupleExpression(std::vector<Expression> &&elements,
                                 const Token &closing_parentheses)
    : elements(std::move(elements)), closing_parentheses(closing_parentheses) {}
const std::string TupleExpression::prettify(void) const {
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
SymbolType TupleExpression::get_type(Analysis::Context &ctx) const {
  UNIMPLEMENTED("Tuple");
}

ScopeResolutionExpression::ScopeResolutionExpression(Expression &&group,
                                                     const Token &colon,
                                                     const Token &member)
    : group(std::move(group)), colon(colon), member(member) {}
const std::string ScopeResolutionExpression::prettify(void) const {
  return std::format("(scope_resolution {} {})", group->prettify(),
                     member.value);
}
SymbolType ScopeResolutionExpression::get_type(Analysis::Context &ctx) const {
  UNIMPLEMENTED("ScopeResolutionExpression");
}

AssignExpression::AssignExpression(Expression &&lhs, const Token &eq,
                                   Expression &&value)
    : lhs(std::move(lhs)), eq(eq), value(std::move(value)) {}

const std::string AssignExpression::prettify(void) const {
  return std::format("(assign {} {} {})", lhs->prettify(), eq.value,
                     value->prettify());
}
SymbolType AssignExpression::get_type(Analysis::Context &ctx) const {
  return value->get_type(ctx);
}
} // namespace AST

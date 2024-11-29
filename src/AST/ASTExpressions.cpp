#include "AST/Expressions.hpp"
#include "AST/TypeNode.hpp"
#include "Analysis/Symbol.hpp"
#include "Analysis/Types.hpp"
#include "common.hpp"
#include "tokens.hpp"
#include <cmath>
#include <cstdlib>
#include <format>
#include <memory>
#include <string>

namespace AST {
using Analysis::NativeType;

void ExpressionNode::analyse(Analysis::Context &ctx) const { UNUSED(ctx); };

std::string ExpressionNone::prettify() const { return "(NONE)"; }
ExpressionNode::TypeResult
ExpressionNone::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  return ExpressionNode::TypeResult();
}

LiteralExpression::LiteralExpression(const Token &v) : value(v) {}
std::string LiteralExpression::prettify(void) const {
  switch (value.type) {
  case TokenType::CharLit:
    return std::format("(char \'{}\')", value.lexem);
  case TokenType::StringLit:
    return std::format("(string \"{}\")", value.lexem);
  case TokenType::IntLit:
    return std::format("(int {})", value.lexem);
  case TokenType::FloatLit:
    return std::format("(float {})", value.lexem);
  default:
    std::exit(70);
  }
}
ExpressionNode::TypeResult
LiteralExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  switch (value.type) {
  case TokenType::CharLit:
    return NativeType::make(NativeType::Kind::Char);
  case TokenType::StringLit:
    return NativeType::make(NativeType::Kind::String);
  case TokenType::IntLit:
    return NativeType::make(NativeType::Kind::Int);
  case TokenType::FloatLit:
    return NativeType::make(NativeType::Kind::Float);
  default:
    return NativeType::make(NativeType::Kind::Unknown);
  }
}

BooleanExpression::BooleanExpression(bool v) : value(v) {}
std::string BooleanExpression::prettify(void) const {
  return value ? "(bool true)" : "(bool false)";
}
ExpressionNode::TypeResult
BooleanExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  return NativeType::make(NativeType::Kind::Bool);
}

UnaryExpression::UnaryExpression(const Token &op, Expression &&rhs)
    : op(op), rhs(std::move(rhs)) {}
std::string UnaryExpression::prettify(void) const {
  return std::format("(unary {} {})", op.lexem, rhs->prettify());
}
ExpressionNode::TypeResult
UnaryExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("UnaryExpression::get_type");
}
void UnaryExpression::analyse(Analysis::Context &ctx) const {
  // TODO: Implement Analyse for Unaries
  rhs->analyse(ctx);
}

BinaryExpression::BinaryExpression(Expression &&lhs, const Token &op,
                                   Expression &&rhs)
    : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}
std::string BinaryExpression::prettify(void) const {
  return std::format("(binary {} {} {})", lhs->prettify(), op.lexem,
                     rhs->prettify());
}
ExpressionNode::TypeResult
BinaryExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("BinaryExpression::get_type");
}
void BinaryExpression::analyse(Analysis::Context &ctx) const {
  // TODO: Analyse binary expressions
  lhs->analyse(ctx);
  rhs->analyse(ctx);
}

GroupingExpression::GroupingExpression(Expression &&expr)
    : expr(std::move(expr)) {}
std::string GroupingExpression::prettify(void) const {
  return std::format("(grouping {})", expr->prettify());
}
ExpressionNode::TypeResult
GroupingExpression::get_type(Analysis::Context &ctx) const {
  return expr->get_type(ctx);
}
void GroupingExpression::analyse(Analysis::Context &ctx) const {
  // TODO
  expr->analyse(ctx);
}

InlineIf::InlineIf(Expression &&condition, Expression &&consequent,
                   Expression &&alternate)
    : condition(std::move(condition)), consequent(std::move(consequent)),
      alternate(std::move(alternate)) {}
std::string InlineIf::prettify(void) const {
  return std::format("(if {} then {} else {})", condition->prettify(),
                     consequent->prettify(), alternate->prettify());
}
ExpressionNode::TypeResult InlineIf::get_type(Analysis::Context &ctx) const {
  return consequent->get_type(ctx);
}
void InlineIf::analyse(Analysis::Context &ctx) const {
  ExpressionNode::TypeResult condition_type = condition->get_type(ctx);
  if (auto cond_t =
          dynamic_cast<Analysis::NativeType *>(condition_type.get())) {
    UNUSED(cond_t);
    UNIMPLEMENTED("InlineIf analysis (condition checking)");
  }
  consequent->analyse(ctx);
  alternate->analyse(ctx);
}

AsExpression::AsExpression(Expression &&expr, TypeNode::Handler &&type)
    : expr(std::move(expr)), type(std::move(type)) {}
std::string AsExpression::prettify(void) const {
  return std::format("(as {} {})", expr->prettify(), type->prettify());
}
ExpressionNode::TypeResult
AsExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  return Type::make(type->get_type());
}
void AsExpression::analyse(Analysis::Context &ctx) const { expr->analyse(ctx); }

MemberAccessExpression::MemberAccessExpression(Expression &&group,
                                               const Token &dot,
                                               const Token &member)
    : group(std::move(group)), dot(dot), member(std::move(member)) {}
std::string MemberAccessExpression::prettify(void) const {
  return std::format("(access {} {})", group->prettify(), member.lexem);
}
ExpressionNode::TypeResult
MemberAccessExpression::get_type(Analysis::Context &ctx) const {
  // TODO: MemberAccessExpression
  UNUSED(ctx);
  UNIMPLEMENTED("MemberAccessExpression");
}

TupleExpression::TupleExpression(std::vector<Expression> &&elements)
    : elements(std::move(elements)) {}
std::string TupleExpression::prettify(void) const {
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
ExpressionNode::TypeResult
TupleExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("Tuple");
}

ScopeResolutionExpression::ScopeResolutionExpression(Expression &&group,
                                                     const Token &colon,
                                                     const Token &member)
    : group(std::move(group)), colon(colon), member(member) {}
std::string ScopeResolutionExpression::prettify(void) const {
  return std::format("(scope_resolution {} {})", group->prettify(),
                     member.lexem);
}
ExpressionNode::TypeResult
ScopeResolutionExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("ScopeResolutionExpression");
}

} // namespace AST

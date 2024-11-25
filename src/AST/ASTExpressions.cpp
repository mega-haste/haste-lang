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
  return std::format("(unary {} {})", op.value, rhs->prettify());
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
  return std::format("(binary {} {} {})", lhs->prettify(), op.value,
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

IdentifierExpression::IdentifierExpression(const Token &v) : value(v) {}
std::string IdentifierExpression::prettify(void) const {
  switch (value.type) {
  case TokenType::Identifier:
    return std::format("(ident {})", value.value);
  case TokenType::SpicialIdentifier:
    return std::format("(spicial_ident {})", value.value);
  default:
    std::exit(70);
  }
}
ExpressionNode::TypeResult
IdentifierExpression::get_type(Analysis::Context &ctx) const {
  if (ctx.is_defined(value)) {
    Analysis::Symbol *symbol = ctx.symbol_table.local_first_look_up(value);
    symbol->uses++;
    return symbol->type;
  }
  return Analysis::TypeError::make(
      std::format("The symbol '{}' aren't defined.", value.value));
}
void IdentifierExpression::analyse(Analysis::Context &ctx) const {
  if (ctx.is_defined(value)) {
    Analysis::Symbol *symbol = ctx.symbol_table.local_first_look_up(value);
    symbol->uses++;
    return;
  }
  ctx.report_error(value, "Use before definition (before initialization)", "");
}

CallExpression::CallExpression(Expression &&callee, const Token &paren,
                               std::vector<Expression> arguments)
    : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {
}
std::string CallExpression::prettify(void) const {
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

ExpressionNode::TypeResult
CallExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("CallExpression::get_type");
}

void CallExpression::analyse(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("CallExpression::analyse");
  // callee->analyse(ctx);
  // ExpressionNode::TypeResult callee_type = callee->get_type(ctx);
  // if (!callee_type->is_function()) {
  //   ctx.report_error(paren, "Type Error", "Should be a function type.");
  //   return;
  // }
  // Analysis::SymbolFunctionType &callee_func =
  //     std::get<Analysis::SymbolFunctionType>(*callee_type->type);
  // if (arguments.size() != callee_func.args.size()) {
  //   ctx.report_error(
  //       paren, "Argument Error",
  //       std::format("Expected {} arguments to be passed, only got {}.",
  //                   callee_func.args.size(), arguments.size()));
  //   return;
  // }
  // for (std::size_t i = 0; i < arguments.size(); i++) {
  //   auto &argument = arguments[i];
  //
  //   argument->analyse(ctx);
  //   ExpressionNode::TypeResult argument_type = argument->get_type(ctx);
  //   auto &target_arg_type = callee_func.args[i];
  //   if (!argument_type->match(target_arg_type.type)) {
  //     ctx.report_error(argument->start, "Argument Error",
  //                      std::format("the {} argument didn't match the expected
  //                      "
  //                                  "type `{}`, got `{}` instead.",
  //                                  i + 1,
  //                                  Analysis::prettify(target_arg_type.type),
  //                                  Analysis::prettify(*argument_type)));
  //   }
  // }
}

MemberAccessExpression::MemberAccessExpression(Expression &&group,
                                               const Token &dot,
                                               const Token &member)
    : group(std::move(group)), dot(dot), member(std::move(member)) {}
std::string MemberAccessExpression::prettify(void) const {
  return std::format("(access {} {})", group->prettify(), member.value);
}
ExpressionNode::TypeResult
MemberAccessExpression::get_type(Analysis::Context &ctx) const {
  // TODO: MemberAccessExpression
  UNUSED(ctx);
  UNIMPLEMENTED("MemberAccessExpression");
}

SubscriptingExpression::SubscriptingExpression(Expression &&item,
                                               const Token &open_brackets,
                                               const Token &close_brackets,
                                               Expression &&index)
    : item(std::move(item)), open_brackets(open_brackets),
      close_brackets(close_brackets), index(std::move(index)) {}
std::string SubscriptingExpression::prettify(void) const {
  return std::format("(subscript {} {})", item->prettify(), index->prettify());
}
ExpressionNode::TypeResult
SubscriptingExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("SubscriptingExpression");
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
                     member.value);
}
ExpressionNode::TypeResult
ScopeResolutionExpression::get_type(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("ScopeResolutionExpression");
}

AssignExpression::AssignExpression(Expression &&lhs, const Token &eq,
                                   Expression &&value)
    : lhs(std::move(lhs)), eq(eq), value(std::move(value)) {}

std::string AssignExpression::prettify(void) const {
  return std::format("(assign {} {} {})", lhs->prettify(), eq.value,
                     value->prettify());
}
ExpressionNode::TypeResult
AssignExpression::get_type(Analysis::Context &ctx) const {
  return lhs->get_type(ctx);
}
void AssignExpression::analyse(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("AssignExpression::analyse")
  // lhs->analyse(ctx);
  // value->analyse(ctx);
  //
  // ExpressionNode::TypeResult lhs_type = lhs->get_type(ctx);
  // ExpressionNode::TypeResult rhs_type = value->get_type(ctx);
  //
  // if (not lhs_type->mut and lhs_type->assigned) {
  //   ctx.report_error(lhs->start, "Double assignment to a immutable
  //   variable.",
  //                    "Cannot assign twice to a immutable. Use `mut` in the "
  //                    "variable declaration.");
  // }
  //
  // if (!lhs_type->match(*rhs_type)) {
  //   ctx.report_error(value->start, "Mismatched types",
  //                    std::format("Expected to be '{}' got '{}'.",
  //                                Analysis::prettify(*lhs_type),
  //                                Analysis::prettify(*rhs_type)));
  // }
  //
  // lhs_type->assigned = true;
}

} // namespace AST

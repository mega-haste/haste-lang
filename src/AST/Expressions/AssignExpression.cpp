
#include "AST/Expressions.hpp"
#include "tokens.hpp"
#include <format>

namespace AST {

AssignExpression::AssignExpression(Expression &&lhs, const Token &eq,
                                   Expression &&value)
    : lhs(std::move(lhs)), eq(eq), value(std::move(value)) {}

std::string AssignExpression::prettify(void) const {
  return std::format("(assign {} {} {})", lhs->prettify(), eq.lexem,
                     value->prettify());
}
ExpressionNode::TypeResult
AssignExpression::get_type(Analysis::Context &ctx) const {
  return lhs->get_type(ctx);
}
void AssignExpression::analyse(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("AssignExpression::analyse")
}

} // namespace AST

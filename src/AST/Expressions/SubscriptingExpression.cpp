
#include "AST/Expressions.hpp"
#include "Analysis/Types.hpp"
#include <format>
namespace AST {

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
  if (Analysis::ArrayType *arr = dynamic_cast<Analysis::ArrayType *>(
          this->item->get_type(ctx).get())) {
    return arr->element_type;
  }
  return Analysis::TypeError::make("Expected to be a subscriptable type.");
}

} // namespace AST

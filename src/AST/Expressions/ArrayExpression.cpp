#include "AST/Expressions.hpp"
#include "Analysis/Types.hpp"
#include <format>
#include <string>

namespace AST {

ArrayExpression::ArrayExpression(std::vector<Expression> &&elements)
    : elements(std::move(elements)) {}

std::string ArrayExpression::prettify(void) const {
  std::string elements_str = "";
  for (auto &element : elements) {
    elements_str.append(element->prettify());
    elements_str.append(", ");
  }
  elements_str.pop_back();
  elements_str.pop_back();

  return std::format("(array [{}])", elements_str);
}

ExpressionNode::TypeResult
ArrayExpression::get_type(Analysis::Context &ctx) const {
  int length = elements.size();
  auto elements_type = elements[0]->get_type(ctx);

  auto res = Analysis::ArrayType::make(elements_type);
  res->length = length;
  return res;
}

void ArrayExpression::analyse(Analysis::Context &ctx) const {
  for (auto &element : elements) {
    element->analyse(ctx);
  }
}

} // namespace AST

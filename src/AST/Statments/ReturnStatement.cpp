
#include "AST/Expressions.hpp"
#include "AST/Statments.hpp"
#include "Analysis/Symbol.hpp"
#include <format>
#include <optional>

namespace AST {

ReturnStatement::ReturnStatement(std::optional<Expression> &&expr)
    : expr(std::move(expr)) {}
std::string ReturnStatement::prettify(const int depth) const {
  return std::format("{}(return {})", repeat_char(' ', depth),
                     expr.has_value() ? expr.value()->prettify() : "NONE");
}
void ReturnStatement::analyse(Analysis::Context &ctx) const {
  Analysis::Symbol *current_function = ctx.get_current_function();
  if (current_function == nullptr) {
    std::cerr << "[" << "??" << ":" << "??" << "] "
              << "`return` should be ALWAYS in a function.\n";
    return;
  }
  if (not expr.has_value() and current_function->type->is_void())
    return;

  ExpressionNode *ex = expr.value().get();
  auto ex_type = ex->get_type(ctx);
  auto fn_type =
      dynamic_cast<Analysis::CallableType *>(current_function->type.get());
  if (not ex_type->is_compatible_with(fn_type->return_type)) {
    ctx.report_error(start, "Mismatch type",
                     std::format("Return type is expected to be `{}` got `{}`.",
                                 fn_type->return_type->stringfy(),
                                 ex_type->stringfy()));
  }
  ex->analyse(ctx);
}

} // namespace AST

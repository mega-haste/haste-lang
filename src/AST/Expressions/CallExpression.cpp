
#include "AST/Expressions.hpp"
#include "Analysis/Types.hpp"
#include <format>
namespace AST {

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
  Analysis::Type::Handler callee_type = callee->get_type(ctx);
  if (Analysis::CallableType *t =
          dynamic_cast<Analysis::CallableType *>(callee_type.get())) {
    return t->return_type;
  }
  return Analysis::TypeError::make("Callee expected to be a function.");
}

void CallExpression::analyse(Analysis::Context &ctx) const {
  callee->analyse(ctx);
  Analysis::Type::Handler callee_type = callee->get_type(ctx);
  if (Analysis::TypeError *err =
          dynamic_cast<Analysis::TypeError *>(callee_type.get())) {
    ctx.report_error(start, "Calling mismatch.", err->msg);
    return;
  }

  Analysis::CallableType *callable =
      dynamic_cast<Analysis::CallableType *>(callee_type.get());
  if (arguments.size() != callable->arguments.size()) {

    ctx.report_error(
        paren, "Argument Error",
        std::format("Expected {} arguments to be passed, only got {}.",
                    callable->arguments.size(), arguments.size()));
    return;
  }
  for (std::size_t i = 0; i < arguments.size(); i++) {
    auto &argument = arguments[i];

    argument->analyse(ctx);
    ExpressionNode::TypeResult argument_type = argument->get_type(ctx);
    auto &target_arg_type = callable->arguments[i];
    if (!argument_type->is_compatible_with(target_arg_type.type)) {
      ctx.report_error(argument->start, "Argument Error",
                       std::format("the {} argument didn't match the expected "
                                   "type `{}`, got `{}` instead.",
                                   i + 1, target_arg_type.type->stringfy(),
                                   argument_type->stringfy()));
    }
  }
}

} // namespace AST


#include "AST/Statments.hpp"
#include "Analysis/Context.hpp"
#include "Analysis/Types.hpp"
#include "tokens.hpp"
#include <format>
#include <vector>

namespace AST {

static inline void
define_all_args(Analysis::Context &ctx,
                std::vector<Analysis::CallableType::Arg> &args) {
  for (auto &arg : args)
    ctx.define(arg.name, arg.type, arg.is_mut);
}

FunctionDef::FunctionDef(const Token &identifier,
                         std::vector<TypedIdentifier> arguments,
                         TypeNode::Handler &&return_type, Statement &&body)
    : identifier(identifier), arguments(std::move(arguments)),
      return_type(std::move(return_type)), body(std::move(body)) {}
std::string FunctionDef::prettify(const int depth) const {
  std::string args;
  std::string body_str;
  for (auto &arg : arguments) {
    args.append(", ");
    args.append(arg.prettify());
  }
  std::string ssss = repeat_char(' ', depth);

  return std::format("{}(func_dec name:{} args:({}) ret_type:{} body:({}))",
                     ssss, identifier.value, args, return_type->prettify(),
                     body->prettify(depth + 4));
}
void FunctionDef::analyse(Analysis::Context &ctx) const {
  auto type = define(ctx);

  ctx.scope_begin();
  define_all_args(ctx, type->arguments);
  ctx.current_func = Analysis::Context::FuncType::Function;

  body->analyse(ctx);

  ctx.current_func = Analysis::Context::FuncType::None;
  ctx.scope_end();
}
Analysis::CallableType::Handler
FunctionDef::define(Analysis::Context &ctx) const {
  ctx.declare(identifier);
  Analysis::CallableType::Handler type =
      Analysis::CallableType::make(return_type->get_type());
  for (auto &argument : arguments) {
    type->arguments.push_back(Analysis::CallableType::Arg(
        argument.identifier, argument.type->get_type(), false));
  }
  return type;
}

} // namespace AST

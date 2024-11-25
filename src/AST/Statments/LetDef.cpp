#include "AST/Statments.hpp"
#include "AST/TypeNode.hpp"
#include "common.hpp"
#include <cmath>
#include <format>

namespace AST {

LetDef::LetDef(const Token &ident, std::optional<Expression> &&value,
               std::optional<TypeNode::Handler> &&type, bool mut)
    : ident(ident), value(std::move(value)), type(std::move(type)), mut(mut) {}
std::string LetDef::prettify(const int depth) const {
  return std::format(
      "{}(let_dec name:{} mut:{} type:{} value:{})", repeat_char(' ', depth),
      ident.value, mut, type.has_value() ? type.value()->prettify() : "unset",
      value.has_value() ? value.value()->prettify() : "undefined");
}
void LetDef::analyse(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("LetDef::analyse")
  // ctx.declare(ident);
  //
  // auto expected_type = IF type.has_value() THEN type.value()->get_type()
  //                          ELSE Analysis::NativeType::Auto;
  // auto value_type = IF value.has_value() THEN value.value()->get_type(
  //     ctx) ELSE ExpressionNode::make_type_result(NativeType::Undefined);
  //
  // if (!value_type->has_error()) {
  //   if (expected_type.is_auto()) {
  //     expected_type = *value_type;
  //
  //     if (expected_type.is_auto() or expected_type.is_undefined()) {
  //       ctx.report_error(
  //           ident, "Unknown type",
  //           "You need either set a known-type value to the "
  //           "variable, or explicitly set the type of the variable.");
  //       return;
  //     }
  //     if (value.has_value())
  //       value.value()->analyse(ctx);
  //     ctx.define(ident, std::move(expected_type), mut);
  //     return;
  //   }
  //
  //   if (value.has_value() and not expected_type.match(*value_type)) {
  //     ctx.report_error(start.line, start.column, start.at, end.at - start.at,
  //                      "Unknown type",
  //                      "Your variable types doesn't match the value type. Try
  //                      " "implicit type inference or use `auto` instead.");
  //     return;
  //   }
  //
  //   expected_type.adjust_with(*value_type);
  // }
  //
  // if (value.has_value()) {
  //   value.value()->analyse(ctx);
  //   expected_type.assigned = true;
  // } else {
  //   expected_type.assigned = false;
  // }
  // ctx.define(ident, std::move(expected_type), mut);
}

} // namespace AST

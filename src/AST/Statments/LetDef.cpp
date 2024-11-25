#include "AST/Statments.hpp"
#include "AST/TypeNode.hpp"
#include "Analysis/Types.hpp"
#include "Reporter.hpp"
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
  ctx.declare(ident);
  Type::Handler variable_type =
      IF type.has_value() THEN type.value()->get_type() ELSE
      Analysis::AutoType::make();
  Type::Handler value_type = IF value.has_value() THEN value.value()->get_type(
      ctx) ELSE Analysis::AutoType::make();

  if (Analysis::TypeError *err =
          dynamic_cast<Analysis::TypeError *>(variable_type.get())) {
    ctx.report_error(type.value()->start, "Type Error", err->msg);
    return;
  }
  if (Analysis::TypeError *err =
          dynamic_cast<Analysis::TypeError *>(value_type.get())) {
    ctx.report_error(value.value()->start, "Type Error", err->msg);
    return;
  }

  if (Analysis::AutoType *_ =
          dynamic_cast<Analysis::AutoType *>(value_type.get())) {
    *variable_type = *value_type;
  } else {
    if (not variable_type->is_compatible_with(value_type)) {
      ctx.report_error(value.value()->start, "Mismatch Type",
                       std::format("`{}` does not match `{}`.",
                                   value_type->stringfy(),
                                   variable_type->stringfy()));
      return;
    }
  }

  variable_type->adjust_with(value_type);

  if (value.has_value()) {
    value.value()->analyse(ctx);
    variable_type->is_assigned = true;
  } else {
    variable_type->is_assigned = false;
  }
  ctx.define(ident, variable_type, mut);
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

Analysis::Type::Handler LetDef::define(Analysis::Context &ctx) const {}

} // namespace AST

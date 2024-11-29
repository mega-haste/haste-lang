#include "AST/Statments.hpp"
#include "AST/TypeNode.hpp"
#include "Analysis/Types.hpp"
#include "common.hpp"
#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>

namespace AST {

LetDef::LetDef(const Token &ident, std::optional<Expression> &&value,
               std::optional<TypeNode::Handler> &&type, bool mut)
    : ident(ident), value(std::move(value)), type(std::move(type)), mut(mut) {}
std::string LetDef::prettify(const int depth) const {
  return std::format(
      "{}(let_dec name:{} mut:{} type:{} value:{})", repeat_char(' ', depth),
      ident.lexem, mut, type.has_value() ? type.value()->prettify() : "unset",
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
          dynamic_cast<Analysis::AutoType *>(variable_type.get())) {
    variable_type = value_type;
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
}

Analysis::Type::Handler LetDef::define(Analysis::Context &ctx) const {
  UNUSED(ctx);
  UNIMPLEMENTED("");
}

} // namespace AST

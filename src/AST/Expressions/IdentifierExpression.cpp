
#include "AST/Expressions.hpp"
#include "tokens.hpp"
#include <format>

namespace AST {

IdentifierExpression::IdentifierExpression(const Token &v) : value(v) {}
std::string IdentifierExpression::prettify(void) const {
  switch (value.type) {
  case TokenType::Identifier:
    return std::format("(ident {})", value.lexem);
  case TokenType::SpicialIdentifier:
    return std::format("(spicial_ident {})", value.lexem);
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
      std::format("The symbol '{}' aren't defined.", value.lexem));
}
void IdentifierExpression::analyse(Analysis::Context &ctx) const {
  if (ctx.is_defined(value)) {
    Analysis::Symbol *symbol = ctx.symbol_table.local_first_look_up(value);
    symbol->uses++;
    return;
  }
  ctx.report_error(value, "Use before definition (before initialization)", "");
}

} // namespace AST

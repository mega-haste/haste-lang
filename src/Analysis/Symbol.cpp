#include "Analysis/Symbol.hpp"
#include "Analysis/Context.hpp"
#include "tokens.hpp"
#include <cstddef>
#include <format>
#include <string>
#include <variant>

namespace Analysis {

Symbol::Symbol(SymbolHandler &&type, bool defined, bool mut)
    : type(std::move(type)), defined(defined), mut(mut) {}
bool Symbol::is_used(void) const { return uses > 0; }
std::string Symbol::prettify() const { return Analysis::prettify(*type); }

SymbolArrayType::SymbolArrayType(SymbolHandler &&type)
    : type(std::move(type)) {}
SymbolArrayType::SymbolArrayType(SymbolHandler &&type, int size, int dimention)
    : type(std::move(type)), size(size), dimention(dimention) {}

SymbolIdentifierType::SymbolIdentifierType(std::string ident) : ident(ident) {}

SymbolFunctionType::Arg::Arg(const Token &ident, SymbolHandler &&type)
    : key(ident), type(std::move(type)) {}
SymbolFunctionType::Arg::Arg(const Token &ident, SymbolHandler &&type,
                             bool has_default)
    : key(ident), type(std::move(type)), has_default(has_default) {}
bool SymbolFunctionType::Arg::operator==(const Arg &other) const {
  return match(type, other.type);
}
bool SymbolFunctionType::Arg::operator!=(const Arg &other) const {
  return not(*this == other);
}

SymbolFunctionType::SymbolFunctionType(SymbolHandler &&return_type)
    : return_type(std::move(return_type)) {}
SymbolFunctionType::SymbolFunctionType(std::vector<Arg> &&args,
                                       SymbolHandler &&return_type)
    : args(std::move(args)), return_type(std::move(return_type)) {}
bool SymbolFunctionType::match_args(
    const std::vector<SymbolFunctionType::Arg> other_args) const {
  if (args.size() != other_args.size())
    return false;
  for (std::size_t i = 0; i < args.size(); i++) {
    auto &my_arg = args[i];
    auto &other_arg = other_args[i];
    if (my_arg != other_arg)
      return false;
  }
  return true;
}
void SymbolFunctionType::define_args(Context &ctx) const {
  for (auto &arg : args) {
    ctx.declare(arg.key);
    ctx.define(arg.key, (SymbolType)*arg.type, false);
  }
}

bool is_native(const SymbolType &symbol) {
  return std::holds_alternative<NativeType>(symbol);
}
bool is_array(const SymbolType &symbol) {
  return std::holds_alternative<SymbolFunctionType>(symbol);
}
bool is_function(const SymbolType &symbol) {
  return std::holds_alternative<SymbolFunctionType>(symbol);
}

bool is_auto(const SymbolType &symbol) {
  if (!is_native(symbol))
    return false;
  NativeType type = std::get<NativeType>(symbol);
  return type == NativeType::Auto;
}

bool is_undefined(const SymbolType &symbol) {
  if (!std::holds_alternative<NativeType>(symbol))
    return false;
  NativeType type = std::get<NativeType>(symbol);
  return type == NativeType::Undefined;
}

bool is_number(const SymbolType &symbol) {
  if (!std::holds_alternative<NativeType>(symbol))
    return false;
  NativeType type = std::get<NativeType>(symbol);
  return type == NativeType::Float || type == NativeType::Int ||
         type == NativeType::UInt || type == NativeType::Char ||
         type == NativeType::Number;
}

bool is_bool(const SymbolType &symbol) {
  if (!std::holds_alternative<NativeType>(symbol))
    return false;
  NativeType type = std::get<NativeType>(symbol);
  return type == NativeType::Bool;
}

bool is_string(const SymbolType &symbol) {
  if (!std::holds_alternative<NativeType>(symbol))
    return false;
  NativeType type = std::get<NativeType>(symbol);
  return type == NativeType::String;
}

bool is(const SymbolType &symbol, const NativeType type) {
  if (!is_native(symbol))
    return false;
  NativeType symbol_type = std::get<NativeType>(symbol);
  return symbol_type == type;
}

bool is(const SymbolType &symbol, const SymbolArrayType type) {
  if (!is_array(symbol))
    return false;
  SymbolArrayType symbol_type = std::get<SymbolArrayType>(symbol);
  return match(symbol_type.type, type.type) && symbol_type.size == type.size &&
         symbol_type.dimention == type.dimention;
}

bool is(const SymbolType &symbol, const SymbolFunctionType type) {
  if (!is_function(symbol))
    return false;
  SymbolFunctionType symbol_type = std::get<SymbolFunctionType>(symbol);
  return symbol_type.match_args(type.args) &&
         match(symbol_type.return_type, type.return_type);
}

bool match(const SymbolHandler &left, const SymbolHandler &right) {
  if (is_native(*left) && is_native(*right))
    return std::get<NativeType>(*left) == std::get<NativeType>(*right);
  if (is_function(*left) && is_function(*right))
    return is(*left, std::get<SymbolFunctionType>(*right));
  if (is_array(*left) && is_array(*right))
    return is(*left, std::get<SymbolArrayType>(*right));
  return false;
}

bool match(const SymbolType &left, const SymbolType &right) {
  if (is_native(left) && is_native(right))
    return std::get<NativeType>(left) == std::get<NativeType>(right);
  if (is_function(left) && is_function(right))
    return is(left, std::get<SymbolFunctionType>(right));
  if (is_array(left) && is_array(right))
    return is(left, std::get<SymbolArrayType>(right));
  return false;
}

SymbolType do_unary(const Token &op, const SymbolType &rhs) {
  switch (op.type) {
  case TokenType::BitwiseNot:
    return NativeType::Number;
  case TokenType::Not:
    return NativeType::Bool;
  case TokenType::Minus:
    if (Analysis::is(rhs, NativeType::UInt))
      return NativeType::UInt;
  case TokenType::Plus:
    if (Analysis::is(rhs, NativeType::Int))
      return NativeType::Int;
    if (Analysis::is(rhs, NativeType::Float))
      return NativeType::Float;
    return NativeType::Number;
  default:
    return NativeType::Unknown;
  }
}

SymbolType do_binary(const SymbolType &lhs, const Token &op,
                     const SymbolType &rhs) {
  switch (op.type) {
  case TokenType::Plus:
  case TokenType::Minus:
  case TokenType::Star:
  case TokenType::FSlash:
  case TokenType::PercentSign:
  case TokenType::DoubleStar:
    return is_number(lhs) && is_number(rhs) ? lhs : NativeType::Unknown;
  case TokenType::EqEq:
  case TokenType::BangEq:
    if (is_string(lhs) && is_string(rhs))
      return lhs;
  case TokenType::LessThan:
  case TokenType::LessThanOrEq:
  case TokenType::GreaterThan:
  case TokenType::GreaterThanOrEq:
    return is_number(lhs) && is_number(rhs) ? lhs : NativeType::Unknown;
  case TokenType::And:
  case TokenType::Or:
    return is_bool(lhs) && is_bool(rhs) ? lhs : NativeType::Unknown;
  default:
    return NativeType::Unknown;
  }
}

std::string prettify(SymbolType &type) {
  if (is_function(type)) {
    SymbolFunctionType fn = std::get<SymbolFunctionType>(type);
    return std::format("func (...): {}", prettify(*fn.return_type));
  }
  if (is_array(type)) {
    SymbolArrayType arr = std::get<SymbolArrayType>(type);
    return std::format("{}[{}]", prettify(*arr.type), arr.size);
  }
  if (is_native(type)) {
    NativeType ntype = std::get<NativeType>(type);
    switch (ntype) {
    case NativeType::Unknown:
      return "unknown";
    case NativeType::Undefined:
      return "undefined";
    case NativeType::Auto:
      return "auto";
    case NativeType::Void:
      return "void";
    case NativeType::Number:
      return "number";
    case NativeType::Int:
      return "int";
    case NativeType::UInt:
      return "uint";
    case NativeType::Float:
      return "float";
    case NativeType::Bool:
      return "bool";
    case NativeType::String:
      return "string";
    case NativeType::Char:
      return "char";
    }
  }
  return "BAD";
}

} // namespace Analysis

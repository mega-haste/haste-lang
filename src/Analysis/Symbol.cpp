#include "Analysis/Symbol.hpp"
#include "Analysis/Context.hpp"
#include "common.hpp"
#include "tokens.hpp"
#include <cstddef>
#include <format>
#include <memory>
#include <string>

namespace Analysis {

SymbolType::SymbolType()
    : type(std::make_shared<SymbolType::Type>(NativeType::Unknown)) {}
SymbolType::SymbolType(const Type &type)
    : type(std::make_shared<SymbolType::Type>(type)) {}
SymbolType::SymbolType(const NativeType &type)
    : type(std::make_shared<SymbolType::Type>(type)) {}

bool SymbolType::is_native() const {
  return std::holds_alternative<NativeType>(*type);
}
bool SymbolType::is_array() const {
  return std::holds_alternative<SymbolFunctionType>(*type);
}
bool SymbolType::is_function() const {
  return std::holds_alternative<SymbolFunctionType>(*type);
}

bool SymbolType::is_auto() const {
  if (!is_native())
    return false;
  NativeType native_type = std::get<NativeType>(*type);
  return native_type == NativeType::Auto;
}
bool SymbolType::is_undefined() const {
  if (!std::holds_alternative<NativeType>(*type))
    return false;
  NativeType native_type = std::get<NativeType>(*type);
  return native_type == NativeType::Undefined;
}
bool SymbolType::is_number() const {
  if (!std::holds_alternative<NativeType>(*type))
    return false;
  NativeType native_type = std::get<NativeType>(*type);
  return native_type == NativeType::Float or native_type == NativeType::Int or
         native_type == NativeType::UInt or native_type == NativeType::Char or
         native_type == NativeType::Number;
}
bool SymbolType::is_bool() const {
  if (!std::holds_alternative<NativeType>(*type))
    return false;
  NativeType native_type = std::get<NativeType>(*type);
  return native_type == NativeType::Bool;
}
bool SymbolType::is_string() const {
  if (!std::holds_alternative<NativeType>(*type))
    return false;
  NativeType native_type = std::get<NativeType>(*type);
  return native_type == NativeType::String;
}

bool SymbolType::is(const NativeType type) const {
  if (!is_native())
    return false;
  NativeType symbol_type = std::get<NativeType>(*this->type);
  return symbol_type == type;
}
bool SymbolType::is(const SymbolArrayType type) const {
  if (!is_array())
    return false;
  SymbolArrayType symbol_type = std::get<SymbolArrayType>(*this->type);
  return symbol_type.type.match(type.type) && symbol_type.size == type.size &&
         symbol_type.dimention == type.dimention;
}
bool SymbolType::is(const SymbolFunctionType type) const {
  if (!is_function())
    return false;
  SymbolFunctionType symbol_type = std::get<SymbolFunctionType>(*this->type);
  return symbol_type.match_args(type.args) && match(type.return_type);
}

bool SymbolType::match(const SymbolType &other) const {
  if (is_native() && other.is_native())
    return std::get<NativeType>(*this->type) ==
           std::get<NativeType>(*other.type);
  if (is_function() && other.is_function())
    return is(std::get<SymbolFunctionType>(*other.type));
  if (is_array() && other.is_array())
    return is(std::get<SymbolArrayType>(*other.type));
  return false;
}

std::string SymbolType::prettify() const {
  return std::string(IF mut THEN "mut" ELSE "") + Analysis::prettify(*this);
}

Symbol::Symbol(SymbolType &&type, bool defined)
    : type(std::make_shared<SymbolType>(std::move(type))), defined(defined) {}
Symbol::Symbol(SymbolType &&type, bool mut, bool defined)
    : type(std::make_shared<SymbolType>(std::move(type))), defined(defined) {
  type.mut = mut;
}
Symbol::Symbol(const SymbolType &type)
    : type(std::make_shared<SymbolType>(type)) {}
Symbol::Symbol(const NativeType &type)
    : type(std::make_shared<SymbolType>(type)) {}
bool Symbol::is_used(void) const { return uses > 0; }
bool Symbol::is_mutable() const { return type->mut; }
std::string Symbol::prettify() const { return type->prettify(); }

SymbolArrayType::SymbolArrayType(SymbolType &&type) : type(std::move(type)) {}
SymbolArrayType::SymbolArrayType(SymbolType &&type, int size, int dimention)
    : type(std::move(type)), size(size), dimention(dimention) {}

SymbolIdentifierType::SymbolIdentifierType(std::string ident) : ident(ident) {}

SymbolFunctionType::Arg::Arg(const Token &ident, SymbolType &&type)
    : key(ident), type(std::move(type)) {}
SymbolFunctionType::Arg::Arg(const Token &ident, SymbolType &&type,
                             bool has_default)
    : key(ident), type(std::move(type)), has_default(has_default) {}
bool SymbolFunctionType::Arg::operator==(const Arg &other) const {
  return type.match(other.type);
}
bool SymbolFunctionType::Arg::operator!=(const Arg &other) const {
  return not(*this == other);
}

SymbolFunctionType::SymbolFunctionType(SymbolType &&return_type)
    : return_type(std::move(return_type)) {}
SymbolFunctionType::SymbolFunctionType(std::vector<Arg> &&args,
                                       SymbolType &&return_type)
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
    ctx.define(arg.key, (SymbolType)arg.type, false);
  }
}

SymbolType do_unary(const Token &op, const SymbolType &rhs) {
  switch (op.type) {
  case TokenType::BitwiseNot:
    return NativeType::Number;
  case TokenType::Not:
    return NativeType::Bool;
  case TokenType::Minus:
    if (rhs.is(NativeType::UInt))
      return NativeType::UInt;
  case TokenType::Plus:
    if (rhs.is(NativeType::Int))
      return NativeType::Int;
    if (rhs.is(NativeType::Float))
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
    return lhs.is_number() && rhs.is_number() ? lhs : NativeType::Unknown;
  case TokenType::EqEq:
  case TokenType::BangEq:
    if (lhs.is_string() && rhs.is_string())
      return lhs;
  case TokenType::LessThan:
  case TokenType::LessThanOrEq:
  case TokenType::GreaterThan:
  case TokenType::GreaterThanOrEq:
    return lhs.is_number() && rhs.is_number() ? lhs : NativeType::Unknown;
  case TokenType::And:
  case TokenType::Or:
    return lhs.is_bool() && rhs.is_bool() ? lhs : NativeType::Unknown;
  default:
    return NativeType::Unknown;
  }
}

std::string prettify(const SymbolType &type) {
  if (type.is_function()) {
    SymbolFunctionType fn = std::get<SymbolFunctionType>(*type.type);
    return std::format("func (...): {}", prettify(fn.return_type));
  }
  if (type.is_array()) {
    SymbolArrayType arr = std::get<SymbolArrayType>(*type.type);
    return std::format("{}[{}]", prettify(arr.type), arr.size);
  }
  if (type.is_native()) {
    NativeType ntype = std::get<NativeType>(*type.type);
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

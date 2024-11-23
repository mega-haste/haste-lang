
#include "AST/TypeNode.hpp"
#include "Analysis/Symbol.hpp"
#include "tokens.hpp"
#include <format>
#include <memory>
#include <optional>
#include <string>

namespace AST {

using Analysis::NativeType;
using Type = std::unique_ptr<TypeNode>;

TypeNode::TypeResult TypeNode::get_type(void) const { return NativeType::Auto; }

TypeLiteral::TypeLiteral(const Token &value) : value(std::move(value)) {}
const std::string TypeLiteral::prettify() const {
  return std::format("(type_lit {})", value.value);
}
TypeNode::TypeResult TypeLiteral::get_type(void) const {
  switch (value.type) {
  case TokenType::Bool:
    return NativeType::Bool;
  case TokenType::Int:
    return NativeType::Int;
  case TokenType::UInt:
    return NativeType::UInt;
  case TokenType::Float:
    return NativeType::Float;
  case TokenType::String:
    return NativeType::String;
  case TokenType::Void:
    return NativeType::Void;
  case TokenType::Auto:
    return NativeType::Auto;
  case TokenType::Char:
    return NativeType::Char;
  default:
    return NativeType::Unknown;
  }
}

ArrayType::ArrayType(Type &&type, std::optional<Token> size)
    : type(std::move(type)), size(size) {}
ArrayType::ArrayType(Type &&type, std::optional<Token> size, int dimention)
    : type(std::move(type)), size(size), dimention(dimention) {}
const std::string ArrayType::prettify() const {
  return std::format("(array {}[{}])", type->prettify(),
                     size.has_value() ? size.value().value : "auto");
}
TypeNode::TypeResult ArrayType::get_type(void) const {
  return TypeNode::TypeResult(Analysis::SymbolArrayType(
      type->get_type(), size.has_value() ? std::stoi(size.value().value) : 1,
      dimention));
}

const std::string UndefinedType::prettify() const {
  return std::format("(undefined_type)");
}
TypeNode::TypeResult UndefinedType::get_type(void) const {
  return NativeType::Undefined;
}

TypedIdentifier::TypedIdentifier(const Token &identifier, Type &&type)
    : identifier(identifier), type(std::move(type)) {}
const std::string TypedIdentifier::prettify() const {
  return std::format("(typed_ident ({}) {})", identifier.value,
                     type->prettify());
}

}; // namespace AST

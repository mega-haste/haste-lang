
#include "AST/TypeNode.hpp"
#include "Analysis/Types.hpp"
#include "tokens.hpp"
#include <charconv>
#include <cstdlib>
#include <format>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>

namespace AST {

using Analysis::NativeType;

TypeNode::TypeResult TypeNode::get_type(void) const {
  return Analysis::AutoType::make();
}

TypeLiteral::TypeLiteral(const Token &value) : value(std::move(value)) {}
const std::string TypeLiteral::prettify() const {
  return std::format("(type_lit {})", value.lexem);
}
TypeNode::TypeResult TypeLiteral::get_type(void) const {
  switch (value.type) {
  case TokenType::Bool:
    return NativeType::make(NativeType::Kind::Bool);
  case TokenType::Int:
    return NativeType::make(NativeType::Kind::Int);
  case TokenType::UInt:
    return NativeType::make(NativeType::Kind::UInt);
  case TokenType::Float:
    return NativeType::make(NativeType::Kind::Float);
  case TokenType::String:
    return NativeType::make(NativeType::Kind::String);
  case TokenType::Void:
    return NativeType::make(NativeType::Kind::Void);
  case TokenType::Auto:
    return Analysis::AutoType::make();
  case TokenType::Char:
    return NativeType::make(NativeType::Kind::Char);
  default:
    return NativeType::make(NativeType::Kind::Unknown);
  }
}

ArrayType::ArrayType(TypeNode::Handler &&type, std::optional<Token> size)
    : type(std::move(type)), size(size) {}
ArrayType::ArrayType(TypeNode::Handler &&type, std::optional<Token> size,
                     int dimention)
    : type(std::move(type)), size(size), dimention(dimention) {}
const std::string ArrayType::prettify() const {
  return std::format("(array {}[{}])", type->prettify(),
                     size.has_value() ? size.value().lexem : "auto");
}
TypeNode::TypeResult ArrayType::get_type(void) const {
  Analysis::ArrayType::Handler res =
      Analysis::ArrayType::make(type->get_type());
  if (size.has_value()) {
    const Token &l = size.value();
    switch (l.type) {
    case TokenType::IntLit: {
      std::from_chars(l.lexem.data(), l.lexem.data() + l.lexem.size(),
                      res->length);
    } break;
    case TokenType::Auto:
      res->length = 0;
      break;
    default:
      std::cerr << "You overcooked" << std::endl;
      std::exit(453);
    }
  }
  return std::move(res);
}

const std::string UndefinedType::prettify() const {
  return std::format("(undefined_type)");
}
TypeNode::TypeResult UndefinedType::get_type(void) const {
  return NativeType::make(NativeType::Kind::Undefined);
}

TypedIdentifier::TypedIdentifier(const Token &identifier,
                                 TypeNode::Handler &&type)
    : identifier(identifier), type(std::move(type)) {}
const std::string TypedIdentifier::prettify() const {
  return std::format("(typed_ident ({}) {})", identifier.lexem,
                     type->prettify());
}

}; // namespace AST

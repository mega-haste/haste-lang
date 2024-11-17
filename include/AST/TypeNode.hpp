#pragma once

#include "Analysis/Symbol.hpp"
#include "tokens.hpp"
#include <format>
#include <macros.hpp>
#include <memory>
#include <optional>
#include <string>

namespace AST {

class TypeNode;

using Analysis::NativeType;
using Analysis::SymbolType;
using Type = std::unique_ptr<TypeNode>;

class TypeNode {
public:
  virtual const std::string prettify() const = 0;
  virtual SymbolType get_type(void) const { return NativeType::Auto; }
};

class TypeLiteral : public TypeNode {
public:
  const Token &value;

  explicit TypeLiteral(const Token &value) : value(std::move(value)) {}

  const std::string prettify() const override {
    return std::format("(type_lit {})", value.value);
  }

  SymbolType get_type(void) const override {
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
};

class ArrayType : public TypeNode {
public:
  Type type;
  std::optional<Token> size;
  int dimention = 1;

  explicit ArrayType(Type &&type, std::optional<Token> size)
      : type(std::move(type)), size(size) {}
  explicit ArrayType(Type &&type, std::optional<Token> size, int dimention)
      : type(std::move(type)), size(size), dimention(dimention) {}

  const std::string prettify() const override {
    return std::format("(array {}[{}])", type->prettify(),
                       size.has_value() ? size.value().value : "auto");
  }
  SymbolType get_type(void) const override {
    return Analysis::SymbolArrayType(
        std::make_unique<SymbolType>(type->get_type()),
        size.has_value() ? std::stoi(size.value().value) : 1, dimention);
  }
};

class UndefinedType : public TypeNode {
public:
  const std::string prettify() const override {
    return std::format("(undefined_type)");
  }
  SymbolType get_type(void) const override { return NativeType::Undefined; }
};

class TypedIdentifier {
public:
  const Token &identifier;
  Type type;

  TypedIdentifier(const Token &identifier, Type &&type)
      : identifier(identifier), type(std::move(type)) {}

  const std::string prettify() const {
    return std::format("(typed_ident ({}) {})", identifier.value,
                       type->prettify());
  }
};

}; // namespace AST

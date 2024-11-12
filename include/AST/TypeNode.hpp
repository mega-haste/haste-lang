#pragma once

#include "tokens.hpp"
#include "Expressions.hpp"
#include <string>
#include <memory>
#include <format>
#include <optional>
#include <macros.hpp>

namespace AST {

class TypeNode;

using Type = std::unique_ptr<TypeNode>;

class TypeNode {
public:
  virtual const std::string prettify() const = 0;
};

class TypeLiteral : public TypeNode {
public:
  const Token &value;

  explicit TypeLiteral(const Token &value) : value(std::move(value)) {}

  const std::string prettify() const override {
    return std::format("(type_lit {})", value.value);
  }
};

class ArrayType : public TypeNode {
public:
  Type type;
  std::optional<Token> size;

  explicit ArrayType(Type &&type, std::optional<Token> size) : type(std::move(type)), size(size) {}

  const std::string prettify() const override {
    return std::format("(array {}[{}])", type->prettify(), size.has_value() ? size.value().value : "auto");
  }
};

class UndefinedType : public TypeNode {
public:
  const std::string prettify() const override {
    return std::format("(undefined_type)");
  }
};

class TypedIdentifier {
public:
  const Token &identifier;
  Type type;

  TypedIdentifier(const Token &identifier, Type &&type) : identifier(identifier), type(std::move(type)) {}

  const std::string prettify() const {
    return std::format("(typed_ident ({}) {})", identifier.value, type->prettify());
  }
};

}; // namespace AST

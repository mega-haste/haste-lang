#pragma once

#include "Analysis/Symbol.hpp"
#include "tokens.hpp"
#include <common.hpp>
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
  using TypeResult = Analysis::SymbolType;

  Token start;
  Token end;

  virtual const std::string prettify() const = 0;
  virtual TypeResult get_type(void) const;

  virtual ~TypeNode() = default;
};

class TypeLiteral : public TypeNode {
public:
  const Token &value;

  explicit TypeLiteral(const Token &value);
  const std::string prettify() const override;
  TypeResult get_type(void) const override;
};

class ArrayType : public TypeNode {
public:
  Type type;
  std::optional<Token> size;
  int dimention = 1;

  explicit ArrayType(Type &&type, std::optional<Token> size);
  explicit ArrayType(Type &&type, std::optional<Token> size, int dimention);
  const std::string prettify() const override;
  TypeResult get_type(void) const override;
};

class UndefinedType : public TypeNode {
public:
  const std::string prettify() const override;
  TypeResult get_type(void) const override;
};

class TypedIdentifier {
public:
  Token start;
  Token end;

  const Token &identifier;
  Type type;

  TypedIdentifier(const Token &identifier, Type &&type);
  const std::string prettify() const;
};

}; // namespace AST

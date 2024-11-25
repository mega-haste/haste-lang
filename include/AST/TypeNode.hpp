#pragma once

#include "Analysis/Types.hpp"
#include "tokens.hpp"
#include <common.hpp>
#include <memory>
#include <optional>
#include <string>

namespace AST {

class TypeNode;

using Analysis::NativeType;
using Analysis::Type;

class TypeNode {
public:
  using TypeResult = Analysis::Type::Handler;
  using Handler = std::shared_ptr<TypeNode>;

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
  TypeNode::Handler type;
  std::optional<Token> size;
  int dimention = 1;

  explicit ArrayType(TypeNode::Handler &&type, std::optional<Token> size);
  explicit ArrayType(TypeNode::Handler &&type, std::optional<Token> size,
                     int dimention);
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
  TypeNode::Handler type;

  TypedIdentifier(const Token &identifier, TypeNode::Handler &&type);
  const std::string prettify() const;
};

}; // namespace AST

#ifndef __SYMBOL_HPP
#define __SYMBOL_HPP

#include "tokens.hpp"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace Analysis {

class SymbolArrayType;
class SymbolIdentifierType;
class SymbolFunctionType;
class Symbol;

class Context;

enum class NativeType : char {
  Unknown = -1,
  Undefined,
  Auto,
  Void,

  // Numiric
  Number, // Geniric Number
  Int,    // sizeof 4
  UInt,   // sizeof 4
  Float,  // sizeof 4
  Bool,   // sizeof 1

  // Text
  String, // variable
  Char,   // sizeof 1
};

class SymbolType {
public:
  using Type = std::variant<NativeType, SymbolArrayType, SymbolIdentifierType,
                            SymbolFunctionType>;
  using Handler = std::shared_ptr<Type>;

  Handler type;
  bool mut = false;
  bool assigned = false;

  SymbolType();
  SymbolType(const Type &type);
  SymbolType(const NativeType &type);

  bool is_native() const;
  bool is_array() const;
  bool is_function() const;

  bool is_auto() const;
  bool is_undefined() const;
  bool is_number() const;
  bool is_bool() const;
  bool is_string() const;

  bool is(const NativeType type) const;
  bool is(const SymbolArrayType type) const;
  bool is(const SymbolFunctionType type) const;

  bool match(const SymbolType &other) const;

  std::string prettify() const;
};

class Symbol {
public:
  std::shared_ptr<SymbolType> type =
      std::make_shared<SymbolType>(NativeType::Unknown);
  bool defined = false;
  int uses = 0;

  Symbol() = default;
  Symbol(SymbolType &&type, bool defined);
  Symbol(SymbolType &&type, bool mut, bool defined);
  Symbol(const SymbolType &type);
  Symbol(const NativeType &type);

  bool is_used(void) const;
  bool is_mutable() const;

  std::string prettify() const;
};

class SymbolArrayType {
public:
  SymbolType type;
  int size = 0;
  int dimention = 1;

  SymbolArrayType(SymbolType &&type);
  SymbolArrayType(SymbolType &&type, int size, int dimention);
};

class SymbolIdentifierType {
public:
  std::string ident;

  SymbolIdentifierType(std::string ident);
};

class SymbolFunctionType {
public:
  struct Arg {
    const Token &key;
    SymbolType type;
    bool has_default = true;

    Arg(const Token &key, SymbolType &&type);
    Arg(const Token &key, SymbolType &&type, bool has_default);

    bool operator==(const Arg &other) const;
    bool operator!=(const Arg &other) const;
  };

  std::vector<Arg> args;
  SymbolType return_type;

  SymbolFunctionType(SymbolType &&return_type);
  SymbolFunctionType(std::vector<Arg> &&args, SymbolType &&return_type);

  bool match_args(const std::vector<Arg> other_args) const;
  void define_args(Context &ctx) const;
};

SymbolType do_unary(const Token &op, const SymbolType &rhs);
SymbolType do_binary(const SymbolType &lhs, const Token &op,
                     const SymbolType &rhs);

std::string prettify(const SymbolType &type);

} // namespace Analysis

#endif // !__SYMBOL_HPP

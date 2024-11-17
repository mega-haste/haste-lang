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

using SymbolType = std::variant<NativeType, SymbolArrayType,
                                SymbolIdentifierType, SymbolFunctionType>;
using SymbolHandler = std::shared_ptr<SymbolType>;

class Symbol {
public:
  SymbolHandler type = std::make_shared<SymbolType>(NativeType::Unknown);
  bool defined = false;
  bool mut = false;
  int uses = 0;

  Symbol();
  Symbol(SymbolHandler &&type, bool defined, bool mut);

  bool is_used(void) const;

  std::string prettify() const;
};

class SymbolArrayType {
public:
  SymbolHandler type;
  int size = 0;
  int dimention = 1;

  SymbolArrayType(SymbolHandler &&type);
  SymbolArrayType(SymbolHandler &&type, int size, int dimention);

  SymbolArrayType(const SymbolArrayType &);
};

class SymbolIdentifierType {
public:
  std::string ident;

  SymbolIdentifierType(std::string ident);
};

class SymbolFunctionType {
public:
  struct Arg {
    std::string key;
    SymbolHandler type;
    bool has_default = true;

    Arg(std::string key, SymbolHandler &&type);
    Arg(std::string key, SymbolHandler &&type, bool has_default);

    bool operator==(const Arg &other) const;
  };

  std::vector<Arg> args;
  SymbolHandler return_type;

  SymbolFunctionType(SymbolHandler &&return_type);
  SymbolFunctionType(std::vector<Arg> &&args, SymbolHandler &&return_type);

  bool match_args(const std::vector<Arg> other_args) const;
  void define_args(Context &ctx) const;
};

bool is_native(const SymbolType &symbol);
bool is_array(const SymbolType &symbol);
bool is_function(const SymbolType &symbol);

bool is_auto(const SymbolType &symbol);
bool is_undefined(const SymbolType &symbol);
bool is_number(const SymbolType &symbol);
bool is_bool(const SymbolType &symbol);

bool is(const SymbolType &symbol, const NativeType type);
bool is(const SymbolType &symbol, const SymbolArrayType type);
bool is(const SymbolType &symbol, const SymbolFunctionType type);

bool match(const SymbolHandler &left, const SymbolHandler &right);
bool match(const SymbolType &left, const SymbolType &right);

SymbolType do_unary(const Token &op, const SymbolType &rhs);
SymbolType do_binary(const SymbolType &lhs, const Token &op,
                     const SymbolType &rhs);

std::string prettify(SymbolType &type);

} // namespace Analysis

#endif // !__SYMBOL_HPP

#ifndef __SYMBOL_HPP
#define __SYMBOL_HPP

#include <initializer_list>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace Analysis {

class Function;
class Variable;
class ArrayType;
class IdentifierType;
class FunctionType;

enum NativeType {
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

using SymbolType =
    std::variant<NativeType, ArrayType, IdentifierType, FunctionType>;
using Symbol = std::tuple<SymbolType, bool>;

class ArrayType {
public:
  SymbolType *type;
  int size = 0;
  int dimention = 1;

  ArrayType(SymbolType *type);
  ArrayType(SymbolType *type, int size, int dimention);

  ~ArrayType();
};

class IdentifierType {
public:
  std::string ident;

  IdentifierType(std::string ident);
};

class FunctionType {
public:
  struct Arg {
    std::string key;
    SymbolType *type;
    bool has_default = true;

    Arg(std::string key, SymbolType *type);
    Arg(std::string key, SymbolType *type, bool has_default);

    ~Arg();
  };

  std::vector<Arg> args;
  SymbolType *return_type;

  FunctionType(SymbolType *return_type);
  FunctionType(std::vector<Arg> args, SymbolType *return_type);
  FunctionType(std::initializer_list<Arg> args, SymbolType *return_type);

  ~FunctionType();
};

} // namespace Analysis

#endif // !__SYMBOL_HPP

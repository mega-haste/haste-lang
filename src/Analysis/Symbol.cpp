#include "Analysis/Symbol.hpp"
#include <iostream>
#include <string>

namespace Analysis {

ArrayType::ArrayType(SymbolType *type) : type(type) {}
ArrayType::ArrayType(SymbolType *type, int size, int dimention)
    : type(type), size(size), dimention(dimention) {}
ArrayType::~ArrayType() { delete this->type; }

IdentifierType::IdentifierType(std::string ident) : ident(ident) {}

FunctionType::Arg::Arg(std::string key, SymbolType *type)
    : key(key), type(type) {}
FunctionType::Arg::Arg(std::string key, SymbolType *type, bool has_default)
    : key(key), type(type), has_default(has_default) {}
FunctionType::Arg::~Arg() {
  std::cout << "delted Arg" << std::endl;
  delete this->type;
}

FunctionType::FunctionType(SymbolType *return_type)
    : return_type(return_type) {}
FunctionType::FunctionType(std::vector<Arg> args, SymbolType *return_type)
    : args(args), return_type(return_type) {}
FunctionType::FunctionType(std::initializer_list<Arg> args,
                           SymbolType *return_type)
    : args(args), return_type(return_type) {}
FunctionType::~FunctionType() {}

} // namespace Analysis

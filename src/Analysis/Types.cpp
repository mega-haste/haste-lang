#include "Analysis/Types.hpp"
#include "common.hpp"
#include "tokens.hpp"
#include <format>
#include <memory>
#include <string>

namespace Analysis {

Type::Handler Type::make(Handler &&type) { return std::move(type); }
bool Type::is_compatible() { UNIMPLEMENTED("Type::is_compatible"); }
bool Type::is_reference() const noexcept { return is_ref; }
bool Type::is_mutable() const noexcept { return is_mut; }

TypeError::Handler TypeError::make(std::string msg) {
  return std::make_shared<TypeError>(msg);
}
TypeError::TypeError(std::string msg) : msg(msg) {}
std::string TypeError::stringfy(void) const noexcept { return msg; }
std::size_t TypeError::get_size(void) const noexcept { return 0; }
std::string TypeError::prettify(void) const noexcept { return "error: " + msg; }

AutoType::Handler AutoType::make() { return std::make_shared<AutoType>(); }
std::string AutoType::stringfy(void) const noexcept { return "auto"; }
std::size_t AutoType::get_size(void) const noexcept { return 0; }

NativeType::Handler NativeType::make() {
  return std::make_shared<NativeType>();
}
NativeType::Handler NativeType::make(Kind kind) {
  return std::make_shared<NativeType>(kind);
}
NativeType::NativeType() {}
NativeType::NativeType(Kind kind) : kind(kind) {}
std::string NativeType::stringfy(void) const noexcept {
  switch (kind) {
  case Kind::Unknown:
    return "unknown";
  case Kind::Undefined:
    return "undefined";
  case Kind::Void:
    return "void";
  case Kind::Number:
    return "number";
  case Kind::Int:
    return "int";
  case Kind::UInt:
    return "uint";
  case Kind::Float:
    return "float";
  case Kind::Bool:
    return "bool";
  case Kind::String:
    return "string";
  case Kind::Char:
    return "char";
  }
}
std::size_t NativeType::get_size(void) const noexcept {
  switch (kind) {
  case Kind::Unknown:
    return 0;
  case Kind::Undefined:
    return 0;
  case Kind::Void:
    return 0;
  case Kind::Number:
    return 8;
  case Kind::Int:
    return 4;
  case Kind::UInt:
    return 4;
  case Kind::Float:
    return 4;
  case Kind::Bool:
    return 1;
  case Kind::String:
    /*
     * string:      // 24
     * len: usize   // 8
     * cap: usize   // 8
     * char*: usize // 8
     */
    return 24;
  case Kind::Char:
    return 1;
  }
}

CallableType::Arg::Arg(Token name, Type::Handler type, bool has_default)
    : name(name), type(type), has_default(has_default) {}
CallableType::Handler CallableType::make() {
  return std::make_shared<CallableType>();
}
CallableType::Handler CallableType::make(Type::Handler return_type) {
  return std::make_shared<CallableType>(return_type);
}
CallableType::CallableType() {}
CallableType::CallableType(Type::Handler return_type)
    : return_type(return_type) {}
std::string CallableType::stringfy(void) const noexcept {
  return std::format("func (...): {}", return_type->stringfy());
}
std::size_t CallableType::get_size(void) const noexcept { return 8; }

InterfaceType::Handler InterfaceType::make(Token name) {
  return std::make_shared<InterfaceType>(name);
}
InterfaceType::InterfaceType(Token name) : name(name) {}
std::string InterfaceType::stringfy(void) const noexcept { return name.value; }
bool InterfaceType::is_implemented_by(Type::Handler other) {
  UNUSED(other);
  UNIMPLEMENTED("InterfaceType::is_implemented_by");
}
std::size_t InterfaceType::get_size(void) const noexcept { return 8; }

StructType::Handler StructType::make(Token name) {
  return std::make_shared<StructType>(name);
}
StructType::StructType(Token name) : name(name) {}
std::string StructType::prettify(void) const noexcept {
  std::string res = "struct " + name.value + " {\n";
  for (auto &field : fields) {
    res.append("  ");
    res.append(field.first.value);
    res.append(": ");
    res.append(field.second->prettify());
    res.append(";\n");
  }
  return res + "}\n";
}
std::string StructType::stringfy(void) const noexcept { return name.value; }
void StructType::add_field(Token name, Type::Handler field) {
  fields[name] = field;
}
void StructType::add_method(Token name, CallableType::Handler method) {
  methods[name] = method;
}
void StructType::implement(Token name, InterfaceType::Handler interface) {
  implementations[name] = interface;
}
std::size_t StructType::get_size(void) const noexcept {
  std::size_t res = 0;
  for (auto &field : fields) {
    res += field.second->get_size();
  }
  return res;
}

ArrayType::Handler ArrayType::make() { return std::make_shared<ArrayType>(); }
ArrayType::Handler ArrayType::make(Type::Handler type) {
  return std::make_shared<ArrayType>(type);
}
ArrayType::ArrayType() {}
ArrayType::ArrayType(Type::Handler type) : element_type(type) {}
std::string ArrayType::stringfy(void) const noexcept {
  return std::format("{}[{}]", element_type->stringfy(),
                     IF length == 0 THEN "auto" ELSE std::to_string(length));
}
std::size_t ArrayType::get_size(void) const noexcept {
  return element_type->get_size() * length;
}

} // namespace Analysis

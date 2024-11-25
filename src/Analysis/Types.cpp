#include "Analysis/Types.hpp"
#include "AST/TypeNode.hpp"
#include "common.hpp"
#include "tokens.hpp"
#include <cstddef>
#include <format>
#include <memory>
#include <string>

namespace Analysis {

static bool is_compatible_bool(NativeType::Kind to);
static bool is_compatible_number(NativeType::Kind to);
static bool isCompatibleInt(NativeType::Kind to);
static bool isCompatibleUInt(NativeType::Kind to);
static bool isCompatibleFloat(NativeType::Kind to);
static bool isCompatibleChar(NativeType::Kind to);
static bool isCompatibleString(NativeType::Kind to);

Type::Handler Type::make(Handler &&type) { return std::move(type); }
bool Type::is_compatible_with(const Type::Handler other) const noexcept {
  UNUSED(other);
  return false;
}
bool Type::is_reference() const noexcept { return is_ref; }
bool Type::is_mutable() const noexcept { return is_mut; }
std::string Type::get_prefixes(void) const {
  return std::string(IF is_ref THEN "&" ELSE "") +
         (IF is_mut THEN "mut" ELSE "") +
         (IF is_ref or is_mut THEN " " ELSE "");
}

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
bool AutoType::is_compatible_with(const Type::Handler other) const noexcept {
  UNUSED(other);
  return true;
}

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
    return get_prefixes() + "unknown";
  case Kind::Undefined:
    return get_prefixes() + "undefined";
  case Kind::Void:
    return get_prefixes() + "void";
  case Kind::Number:
    return get_prefixes() + "number";
  case Kind::Int:
    return get_prefixes() + "int";
  case Kind::UInt:
    return get_prefixes() + "uint";
  case Kind::Float:
    return get_prefixes() + "float";
  case Kind::Bool:
    return get_prefixes() + "bool";
  case Kind::String:
    return get_prefixes() + "string";
  case Kind::Char:
    return get_prefixes() + "char";
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
bool NativeType::is_compatible_with(const Type::Handler other) const noexcept {
  if (NativeType *o = dynamic_cast<NativeType *>(other.get())) {
    if (o->kind == kind)
      return true;
    switch (kind) {
    case Kind::Number:
      return is_compatible_number(o->kind);
    case Kind::Bool:
      return is_compatible_bool(o->kind);
    case Kind::Int:
      return isCompatibleInt(o->kind);
    case Kind::UInt:
      return isCompatibleUInt(o->kind);
    case Kind::Float:
      return isCompatibleFloat(o->kind);
    case Kind::Char:
      return isCompatibleChar(o->kind);
    case Kind::String:
      return isCompatibleString(o->kind);
    case Kind::Unknown:
    case Kind::Undefined:
    case Kind::Void:
      return false;
    }
  }
  return false;
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
  return get_prefixes() +
         std::format("func (...): {}", return_type->stringfy());
}
std::size_t CallableType::get_size(void) const noexcept { return 8; }
bool CallableType::is_compatible_with(
    const Type::Handler other) const noexcept {
  if (CallableType *o = dynamic_cast<CallableType *>(other.get())) {
    if (arguments.size() != o->arguments.size())
      return false;
    if (not return_type->is_compatible_with(o->return_type))
      return false;

    for (std::size_t i = 0; i < arguments.size(); i++) {
      if (not arguments[i].type->is_compatible_with(o->arguments[i].type))
        return false;
    }
    return true;
  }
  return false;
}

InterfaceType::Handler InterfaceType::make(Token name) {
  return std::make_shared<InterfaceType>(name);
}
InterfaceType::InterfaceType(Token name) : name(name) {}
std::string InterfaceType::stringfy(void) const noexcept {
  return get_prefixes() + name.value;
}
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
std::string StructType::stringfy(void) const noexcept {
  return get_prefixes() + name.value;
}
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

EnumType::Handler EnumType::make(Token name) {
  return std::make_shared<EnumType>(name);
}
EnumType::Handler EnumType::make(Token name, Type::Handler member_type) {
  return std::make_shared<EnumType>(name, member_type);
}

EnumType::EnumType(Token name) : name(name) {}
EnumType::EnumType(Token name, Type::Handler member_type)
    : name(name), member_type(member_type) {}

std::string EnumType::prettify(void) const noexcept {
  std::string res = "enum " + name.value + " {\n";
  for (auto &member : members) {
    res.append("  ");
    res.append(member.first.value);
    res.append(" = ");
    res.append(std::to_string(member.second));
    res.append(",\n");
  }
  return res + "}\n";
}
std::string EnumType::stringfy(void) const noexcept { return name.value; }
std::size_t EnumType::get_size(void) const noexcept {
  return member_type->get_size();
}

void EnumType::add_member(Token name) { members[name] = last_value++; }
void EnumType::add_member(Token name, int value) {
  members[name] = last_value = value;
}
bool EnumType::has_member(Token name) const noexcept {
  return members.contains(name);
}

ArrayType::Handler ArrayType::make() { return std::make_shared<ArrayType>(); }
ArrayType::Handler ArrayType::make(Type::Handler type) {
  return std::make_shared<ArrayType>(type);
}
ArrayType::ArrayType() {}
ArrayType::ArrayType(Type::Handler type) : element_type(type) {}
std::string ArrayType::stringfy(void) const noexcept {
  return get_prefixes() +
         std::format("{}[{}]", element_type->stringfy(),
                     IF length == 0 THEN "auto" ELSE std::to_string(length));
}
std::size_t ArrayType::get_size(void) const noexcept {
  return element_type->get_size() * length;
}
bool ArrayType::is_compatible_with(const Type::Handler other) const noexcept {
  if (ArrayType *o = dynamic_cast<ArrayType *>(other.get())) {
    return element_type->is_compatible_with(o->element_type) and
           (length == 0 or length == o->length);
  }
  return false;
}

static bool is_compatible_bool(NativeType::Kind to) {
  UNUSED(to);
  return false;
}

static bool is_compatible_number(NativeType::Kind to) {
  switch (to) {
  case NativeType::Kind::UInt:
  case NativeType::Kind::Int:
  case NativeType::Kind::Float:
  case NativeType::Kind::Char:
    return true;
  default:
    return false;
  }
}

static bool isCompatibleInt(NativeType::Kind to) {
  switch (to) {
  case NativeType::Kind::UInt:
  case NativeType::Kind::Float:
  case NativeType::Kind::Char:
    return true;
  default:
    return false;
  }
}

// Check if UInt is compatible with other types
static bool isCompatibleUInt(NativeType::Kind to) {
  switch (to) {
  case NativeType::Kind::Int:
  case NativeType::Kind::Float:
  case NativeType::Kind::Char:
    return true;
  default:
    return false;
  }
}

// Check if Float is compatible with other types
static bool isCompatibleFloat(NativeType::Kind to) {
  switch (to) {
  case NativeType::Kind::Int:
  case NativeType::Kind::UInt:
  case NativeType::Kind::Char:
    return true;
  default:
    return false;
  }
}

// Check if Char is compatible with other types
static bool isCompatibleChar(NativeType::Kind to) {
  switch (to) {
  case NativeType::Kind::Int:
  case NativeType::Kind::Float:
  case NativeType::Kind::UInt:
    return true;
  default:
    return false;
  }
}

// Check if String is compatible with other types
static bool isCompatibleString(NativeType::Kind to) {
  UNUSED(to);
  return false;
}

} // namespace Analysis

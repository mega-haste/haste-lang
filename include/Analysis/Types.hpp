#ifndef __TYPES_HPP
#define __TYPES_HPP

#include "common.hpp"
#include "tokens.hpp"
#include <cstddef>
#include <cstdint>
#include <linux/limits.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace Analysis {

struct Type {
  using Handler = std::shared_ptr<Type>;

  bool is_ref = false;
  bool is_mut = false;
  bool is_assigned = false;

  virtual ~Type() = default;
  virtual std::string prettify(void) const noexcept { return stringfy(); }
  virtual std::string stringfy(void) const noexcept = 0;
  virtual std::size_t get_size(void) const noexcept = 0;
  virtual void adjust_with(const Type::Handler other) noexcept {
    UNUSED(other);
  }
  virtual bool is_compatible_with(const Type::Handler other) const noexcept;
  std::string get_prefixes(void) const;

  static Handler make(Handler &&type);

  bool is_reference() const noexcept;
  bool is_mutable() const noexcept;
  bool is_void() const noexcept;
};

struct TypeError : public Type {
  using Handler = std::shared_ptr<TypeError>;

  std::string msg = 0;

  static Handler make(std::string msg);
  TypeError(std::string msg);

  std::string prettify(void) const noexcept override;
  std::string stringfy(void) const noexcept override;
  std::size_t get_size(void) const noexcept override;
};

struct AutoType : public Type {
  using Handler = std::shared_ptr<AutoType>;

  static Handler make();
  std::string stringfy(void) const noexcept override;
  std::size_t get_size(void) const noexcept override;
  // void adjust_with(const Type::Handler other) noexcept override;
  bool is_compatible_with(const Type::Handler other) const noexcept override;
};

struct NativeType : public Type {
  using Handler = std::shared_ptr<NativeType>;
  enum class Kind {
    Unknown = -1,
    Undefined,
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

  Kind kind = Kind::Unknown;

  static Handler make();
  static Handler make(Kind kind);

  NativeType();
  NativeType(Kind kind);

  std::string stringfy(void) const noexcept override;
  std::size_t get_size(void) const noexcept override;
  bool is_compatible_with(const Type::Handler other) const noexcept override;
};

struct CallableType : public Type {
  using Handler = std::shared_ptr<CallableType>;
  struct Arg {
    Token name;
    Type::Handler type;
    bool has_default = false;
    bool is_mut = false;

    Arg(Token name, Type::Handler type, bool has_default);
  };

  std::vector<Arg> arguments;
  Type::Handler return_type = NativeType::make(NativeType::Kind::Void);

  static Handler make();
  static Handler make(Type::Handler return_type);

  CallableType();
  CallableType(Type::Handler return_type);

  std::string stringfy(void) const noexcept override;
  std::size_t get_size(void) const noexcept override;
  bool is_compatible_with(const Type::Handler other) const noexcept override;
};

struct InterfaceType : public Type {
  using Handler = std::shared_ptr<InterfaceType>;
  Token name;
  std::unordered_map<Token, Type::Handler> methods;

  static Handler make(Token name);
  InterfaceType(Token name);

  // std::string prettify(void) const noexcept override;
  std::string stringfy(void) const noexcept override;
  std::size_t get_size(void) const noexcept override;

  bool is_implemented_by(Type::Handler other);
};

struct StructType : public Type {
  using Handler = std::shared_ptr<StructType>;
  Token name;
  std::unordered_map<Token, Type::Handler> fields;
  std::unordered_map<Token, Type::Handler> methods;
  std::unordered_map<Token, InterfaceType::Handler> implementations;

  static Handler make(Token name);

  StructType(Token name);

  std::string prettify(void) const noexcept override;
  std::string stringfy(void) const noexcept override;
  std::size_t get_size(void) const noexcept override;

  void add_field(Token name, Type::Handler field);
  void add_method(Token name, CallableType::Handler method);
  void implement(Token name, InterfaceType::Handler interface);
};

struct EnumType : public Type {
  using Handler = std::shared_ptr<EnumType>;
  using Member = std::uint64_t;
  // struct Member {
  //   Token name;
  //   int value = 0;
  //
  //   Member(Token name, int value = 0);
  // };

  Token name;
  Type::Handler member_type = NativeType::make(NativeType::Kind::Int);
  std::unordered_map<Token, Member> members;
  Member last_value = 0;

  static Handler make(Token name);
  static Handler make(Token name, Type::Handler member_type);

  EnumType(Token name);
  EnumType(Token name, Type::Handler member_type);

  std::string prettify(void) const noexcept override;
  std::string stringfy(void) const noexcept override;
  std::size_t get_size(void) const noexcept override;

  void add_member(Token name);
  void add_member(Token name, int value);

  bool has_member(Token name) const noexcept;
};

struct ArrayType : public Type {
  using Handler = std::shared_ptr<ArrayType>;
  Type::Handler element_type = NativeType::make(NativeType::Kind::Undefined);
  std::size_t length = 0;

  static Handler make();
  static Handler make(Type::Handler type);

  ArrayType();
  ArrayType(Type::Handler type);

  std::string stringfy(void) const noexcept override;
  std::size_t get_size(void) const noexcept override;
  bool is_compatible_with(const Type::Handler other) const noexcept override;
  void adjust_with(const Type::Handler other) noexcept override {
    if (ArrayType *o = dynamic_cast<ArrayType *>(other.get())) {
      length = o->length;
    }
  }
};

} // namespace Analysis

#endif // !__TYPES_HPP

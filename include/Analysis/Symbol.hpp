#ifndef __SYMBOL_HPP
#define __SYMBOL_HPP

#include "Analysis/Types.hpp"
#include <string>

namespace Analysis {

class Symbol {
public:
  Type::Handler type = NativeType::make();
  bool defined = false;
  bool mut = false;
  int uses = 0;

  Symbol() = default;
  Symbol(Type::Handler type);
  Symbol(Type::Handler type, bool defined);
  Symbol(Type::Handler type, bool defined, bool mut);

  bool is_used(void) const noexcept;
  bool is_mutable() const noexcept;
  bool is_reference() const noexcept;

  std::string prettify() const;
};

} // namespace Analysis

#endif // !__SYMBOL_HPP

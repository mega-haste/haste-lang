#include "Analysis/Symbol.hpp"
#include <format>

namespace Analysis {
Symbol::Symbol(Type::Handler type) : type(type) {}
Symbol::Symbol(Type::Handler type, bool defined)
    : type(type), defined(defined) {}
Symbol::Symbol(Type::Handler type, bool defined, bool mut)
    : type(type), defined(defined), mut(mut) {}

bool Symbol::is_used(void) const noexcept { return uses > 0; }
bool Symbol::is_mutable() const noexcept { return mut; }
bool Symbol::is_reference() const noexcept { return type->is_ref; }

std::string Symbol::prettify() const {
  return std::format("(Symbol defined:{} mut:{} uses:{} type:{})", defined, mut,
                     uses, type->stringfy());
}

} // namespace Analysis

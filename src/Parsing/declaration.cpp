#include "Parser.hpp"
#include "tokens.hpp"

Statement Parser::declaration(void) {
  try {
    if (match({TokenType::Let}))
      return let();
    if (match({TokenType::Func}))
      return function();

    return statement();
  } catch (ParserError &e) {
    synchronize();
    return nullptr;
  }
}

void Parser::static_declaration(TranslationUnit &tu) {
  try {
    if (match({TokenType::Func})) {
      auto v = function();
      tu.add_function(std::move(v));
    } else {
      throw error(peek(), "Only allowed `func`, `struct`, `import`, `const` and `use` on the top level or whatever you call it.");
    }
  } catch (ParserError &e) {
    static_synchronize();
  }
}

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

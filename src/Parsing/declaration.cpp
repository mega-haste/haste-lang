#include "AST/Statments.hpp"
#include "Parser.hpp"
#include "tokens.hpp"
#include <memory>

Statement Parser::declaration(void) {
  try {
    if (match({TokenType::Let}))
      return let();
    if (match({TokenType::Func}))
      return function();

    return statement();
  } catch (ParserError &e) {
    report_error(e);
    synchronize();
  }

  return std::make_unique<NoneStmt>();
}

void Parser::static_declaration(Program &program) {
  try {
    if (match({TokenType::Func})) {
      program.add_function(function());
    } else {
      throw error(peek(),
                  "Only allowed `func`, `struct`, `import`, `const` and `use` "
                  "on the top level. got {}.");
    }
  } catch (ParserError &e) {
    report_error(e);
    static_synchronize();
  }
}

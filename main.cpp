
#include "Analysis/Types.hpp"
#include "common.hpp"
#include "tokens.hpp"
#include <iostream>

using namespace Analysis;

int main(int argc, char *argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  NativeType::Handler integer = NativeType::make(NativeType::Kind::Int);
  NativeType::Handler character = NativeType::make(NativeType::Kind::Char);
  NativeType::Handler string_t = NativeType::make(NativeType::Kind::String);
  auto t = ArrayType::make(character);
  t->length = 50;

  Token n(TokenType::Identifier, "Person");

  auto person_struct = StructType::make(n);
  person_struct->add_field(Token(TokenType::Identifier, "name"), string_t);
  person_struct->add_field(Token(TokenType::Identifier, "age"), integer);
  person_struct->add_field(Token(TokenType::Identifier, "permetions"), t);

  auto person_array = ArrayType::make(person_struct);
  person_array->length = 60;

  std::cout << person_struct->prettify() << person_struct->stringfy()
            << " size: " << person_struct->get_size() << "\n";
  std::cout << person_array->prettify() << " size: " << person_array->get_size()
            << "\n";

  // Compiler compiler;
  // compiler.compile("./main.haste");
  // compiler.analyse_all();

  return 0;
}

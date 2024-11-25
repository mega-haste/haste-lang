
#include "Compiler.hpp"
#include "common.hpp"

using namespace Analysis;

int main(int argc, char *argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  Compiler compiler;
  compiler.compile("./main.haste");
  compiler.analyse_all();

  return 0;
}

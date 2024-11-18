#include "Compiler.hpp"

int main(int argc, char *argv[]) {
  Compiler compiler;
  compiler.compile("./main.haste");
  compiler.analyse_all();
  return 0;
}

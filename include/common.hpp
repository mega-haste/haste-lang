#pragma once

#include <iostream>

#define IF
#define THEN ?
#define ELSE :

#define UNUSED(expr)                                                           \
  do {                                                                         \
    (void)(expr);                                                              \
  } while (0)

#define LOG(msg) std::cout << "LOG: " << msg << '\n';
#define HERE() LOG("Here!")
#define UNIMPLEMENTED(tag)                                                     \
  std::cout << "Unimplemented: " << tag << std::endl;                          \
  exit(1);
#define TRY_OR_REPORT(stmt)                                                    \
  try {                                                                        \
    stmt;                                                                      \
  } catch (ParserError & e) {                                                  \
    report_error(e);                                                           \
    synchronize();                                                             \
  }
#define TRY_OR_REPORT_NO_SYNC(stmt)                                            \
  try {                                                                        \
    stmt;                                                                      \
  } catch (ParserError & e) {                                                  \
    report_error(e);                                                           \
  }

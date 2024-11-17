#ifndef __TRANSLATION_UNIT__CPP
#define __TRANSLATION_UNIT__CPP

#include "AST/Statments.hpp"
#include "Analysis/Context.hpp"

class TranslationUnit {
public:
  std::vector<std::unique_ptr<AST::FunctionDef>> functions;

  void add_function(std::unique_ptr<AST::FunctionDef> func) {
    functions.push_back(std::move(func));
  }

  void pretty_print(void) const {
    for (auto &function : functions) {
      std::cout << function->prettify() << '\n';
    }
  }

  void analyse() {
    Analysis::Context ctx;
    for (auto &fn : functions) {
      fn->analyse(ctx);
    }

    ctx.report_summary();
  }
};

#endif

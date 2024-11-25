#ifndef __PARSER_HPP
#define __PARSER_HPP

#include "AST/Expressions.hpp"
#include "AST/Statments.hpp"
#include "AST/TypeNode.hpp"
#include "Program.hpp"
#include "tokens.hpp"
#include <cstddef>
#include <initializer_list>
#include <memory>

using namespace AST;

constexpr uint MAX_PRAM_COUT = 128;
constexpr uint MAX_TUPLE_COUT = 64;

class ParserError {
public:
  const Token &token;
  const char *message;

  ParserError(const Token &token, const char *message);
};

class Parser {
public:
  Parser(const TokenList &tokens);

  void parse(Program &program);

private:
  static inline Token f = Token(TokenType::Void, "void");
  const TokenList &m_tokens;
  std::size_t m_current = 0;
  bool m_had_error = false;

  Statement declaration(void);
  void static_declaration(Program &program);
  Statement statement(void);

  std::unique_ptr<FunctionDef> function(void);
  Statement let(void);

  std::vector<Statement> block(void);
  Statement block_statement(void);
  Statement if_condition(void);
  Statement $return(void);

  TypedIdentifier parse_typed_ident(void);
  TypeNode::Handler parse_type(void);

  Statement expression_statement(void);

  Expression expression(void);
  Expression assignment(void);
  Expression inline_if(void);
  Expression as(void);
  Expression $or(void);
  Expression $and(void);
  Expression bitwise_and(void);
  Expression bitwise_or(void);
  Expression equality(void);
  Expression relational(void);
  Expression bitwise_shift(void);
  Expression addition(void);
  Expression multiplication(void);
  Expression power(void);
  Expression nuts(void); // 🥜
  Expression unary(void);
  Expression call(void);
  Expression finish_call(Expression &&callee);
  Expression scope_resolution(void);
  Expression primary(void);

  Expression parse_tuple(std::vector<Expression> &&exprs);
  Expression parse_array();

  bool match(std::initializer_list<TokenType> types);
  bool check(TokenType type) const;
  bool is_at_end(void) const;

  const Token &consume_semi_colon(const char *message);
  const Token &consume_identifier(const char *message);

  const Token &consume(TokenType type, const char *message);
  const Token &peek(void) const;
  const Token &peek(int ahead) const;
  const Token &previous(void) const;
  const Token &advance(void);
  ParserError error(const Token &token, const char *message);

  void report_error(ParserError &error);

  void synchronize(void);
  void static_synchronize(void);
};

#endif

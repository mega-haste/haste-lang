#ifndef __PARSER_HPP
#define __PARSER_HPP

#include "AST/Expressions.hpp"
#include "AST/Statments.hpp"
#include "tokens.hpp"
#include <cstddef>
#include <initializer_list>

using namespace AST;

constexpr uint MAX_PRAM_COUT  = 128;
constexpr uint MAX_TUPLE_COUT = 60;

class ParserError {
  public:
    Token token;
    const char *message;

    ParserError(Token token, const char *message);
};

class Parser {
  public:
    Parser(TokenList tokens);

    std::vector<Statement> parse();

  private:
    static inline Token f = Token(TokenType::Void, "void");
    TokenList m_tokens;
    std::size_t m_current = 0;

    Statement declaration(void);
    Statement statement(void);
    Statement function(void);

    std::vector<Statement> block(void);

    TypedIdentifier parse_typed_ident(void);
    Type parse_type(void);

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

    const bool match(std::initializer_list<TokenType> types);
    const bool check(TokenType type) const;
    const bool is_at_end(void) const;

    const Token &consume_semi_colon(void);
    const Token &consume_identifier(const char *message);

    const Token &consume(TokenType type, const char *message);
    const Token &peek(void) const;
    const Token &peek(int ahead) const;
    const Token &previous(void) const;
    const Token &advance(void);
    ParserError error(Token token, const char *message);

    void synchronize(void);
};

#endif
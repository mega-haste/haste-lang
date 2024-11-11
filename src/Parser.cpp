#include "Parser.hpp"
#include "AST/Expressions.hpp"
#include "tokens.hpp"
#include "AST/sigma.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

using namespace AST;

ParserError::ParserError(Token token, const char *message)
    : token(token), message(message) {}

Parser::Parser(TokenList tokens) : m_tokens(tokens) {}

std::vector<Statement> Parser::parse() {
  std::vector<Statement> statements;
  while (!is_at_end()) {
    statements.push_back(declaration());
  }

  return statements;
}

const bool Parser::match(std::initializer_list<TokenType> types) {
  for (TokenType type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

const bool Parser::check(TokenType type) const {
  return is_at_end() ? false : peek() == type;
}

const Token &Parser::consume(TokenType type, const char *message) {
  if (check(type))
    return advance();

  throw error(peek(), message);
}

const bool Parser::is_at_end(void) const { return peek() == TokenType::_EOF; }

const Token &Parser::consume_semi_colon(void) {
  return consume(TokenType::SemiColon, "Expected ';' after end of a statment.");
}

const Token &Parser::consume_identifier(const char *message) {
  if (check(TokenType::Identifier) || check(TokenType::SpicialIdentifier))
    return advance();
  
  throw error(peek(), message);
}

const Token &Parser::peek(void) const { return m_tokens.at(m_current); }
const Token &Parser::peek(int ahead) const { return m_tokens.at(m_current + ahead); }

const Token &Parser::previous(void) const { return m_tokens.at(m_current - 1); }

const Token &Parser::advance(void) {
  if (!is_at_end())
    m_current++;
  return previous();
}

ParserError Parser::error(Token token, const char *message) {
  return ParserError(token, message);
}

void Parser::synchronize(void) {
  advance();
  // TODO
}

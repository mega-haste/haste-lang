#pragma once

#include "tokens.hpp"
#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Scanner {
public:
  Scanner();
  Scanner(const std::string_view content, std::vector<std::string_view> *lines);

  TokenList scan(void);
  void scan_line(void);
  void reconstruct(const std::string_view content);

  static void setup_keywords();

private:
  struct Ident {
    std::string value;
    Scanner *scanner;

    Ident(Scanner *it_scanner, std::string value)
        : value(value), scanner(it_scanner) {}

    void skip() { scanner->m_current += value.length(); }

    std::string to_string() {
      std::stringstream stream;
      stream << "Ident(";
      stream << value;
      stream << ")";
      return stream.str();
    }
  };

  inline static std::unordered_map<std::string_view, TokenType> keywords =
      std::unordered_map<std::string_view, TokenType>();
  std::vector<std::string_view> *m_lines;
  std::vector<Token> m_tokens;
  std::string_view m_content;
  std::size_t m_current = 0;
  std::size_t m_start = 0;
  std::size_t m_line = 1;
  std::size_t m_column = 1;
  std::size_t m_line_starting = 0;

  void scan_ident();
  void scan_special();
  void scan_numbers();
  void scan_string();

  void add_token(TokenType type);
  void add_token(TokenType type, std::string_view value);

  bool ident_match(std::string ident);
  bool is(char c);
  bool is_alpha();
  bool is_digit();
  bool is_alphanum();

  bool is_alpha(char c);
  bool is_digit(char c);
  bool is_alphanum(char c);

  bool match(char a);
  bool at_end();
  bool at_end(std::size_t i);
  char peek();
  char peek(int c);
  char advance();
  char advance(std::size_t i);

  Ident get_next_ident();
};

#pragma once

#include <cstddef>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

enum class TokenType {
  _EOF = -1,

  // tokens
  OpenParen = 0,      // '('
  CloseParen,         // ')'
  OpenCurlyBrase,     // '{'
  CloseCurlyBrase,    // '}'
  OpenSquareBracket,  // '['
  CloseSquareBracket, // ']'

  Plus,        // '+'
  Minus,       // '-'
  Star,        // '*'
  DoubleStar,  // '**'
  FSlash,      // '/'
  PercentSign, // '%'

  BSlash, // '\'

  Eq,              // '='
  PlusEq,          // '+='
  MinusEq,         // '-='
  StarEq,          // '*='
  DoubleStarEq,    // '**='
  FSlashEq,        // '+='
  PercentSignEq,   // '%='
  EqEq,            // '=='
  BangEq,          // '!='
  LessThan,        // '<'
  GreaterThan,     // '>'
  LessThanOrEq,    // '<='
  GreaterThanOrEq, // '>='

  Not, // 'not'
  And, // 'and'
  Or,  // 'or'

  BitwiseAnd,        // '&'
  BitwiseOr,         // '|'
  BitwiseNot,        // '~'
  BitWiseLeftShift,  // '<<'
  BitWiseRightShift, // '<<'

  Dot,        // '.'
  SemiColon,  // ';'
  Comma,      // ','
  Colon,      // ':'
  ColonColon, // '::'

  // Literals
  IntLit,   // [0-9]*
  FloatLit, // [0-9]*\.[0-9]*
  ComplexLit,
  StringLit, // "*"
  CharLit,   // '\c' // I mean one character :D
  Identifier,
  SpicialIdentifier,

  // Keywords
  True,
  False,

  As,
  Mut,
  Let,
  Func,
  InlineFunc, // !func
  Return,

  // Type keywords
  Char,
  String,
  UInt,
  Int,
  Float,
  Bool,
  Void,
  Auto,

  //// Control flows
  If,
  Then,
  Unless, // !if
  Else,
  Is,
  For,
  Loop,
  While,

  //// Class related
  Class,
  FinalClass, // !class

  Try,
  Catch,
};

struct Token {
  TokenType type = (TokenType)0;
  std::string_view lexem;
  std::size_t line = 1;
  std::size_t column = 1;
  std::size_t at = 0;
  std::size_t length = 0;

  Token() = default;
  Token(TokenType type) : type(type) {}
  Token(TokenType type, std::string_view value) : type(type), lexem(value) {}
  Token(TokenType type, std::string_view value, std::size_t line)
      : type(type), lexem(value), line(line) {}
  Token(TokenType type, std::string_view value, std::size_t line,
        std::size_t column)
      : type(type), lexem(value), line(line), column(column) {}
  Token(TokenType type, std::string_view value, std::size_t line,
        std::size_t column, std::size_t at)
      : type(type), lexem(value), line(line), column(column), at(at) {}
  Token(TokenType type, std::string_view value, std::size_t line,
        std::size_t column, std::size_t at, std::size_t length)
      : type(type), lexem(value), line(line), column(column), at(at),
        length(length) {}
  ~Token() = default;

  bool operator==(const TokenType &other) const { return type == other; }
  bool operator==(const std::string &other) const { return lexem == other; }
  bool operator==(const Token &other) const {
    return type == other.type && lexem == other.lexem;
  }

  std::string to_string() const {
    std::stringstream stream;
    stream << "Token(";
    stream << token_tostring(type);
    stream << ", \"";
    stream << lexem;
    stream << "\", ";
    stream << std::to_string(line);
    stream << ", ";
    stream << std::to_string(column);
    stream << ")";
    return stream.str();
  }

  static std::string token_tostring(TokenType token) {
    switch (token) {
#define TM(token)                                                              \
  case TokenType::token:                                                       \
    return #token;
      TM(OpenParen)
      TM(CloseParen)
      TM(OpenCurlyBrase)
      TM(CloseCurlyBrase)
      TM(OpenSquareBracket)
      TM(CloseSquareBracket)

      TM(Plus)
      TM(Minus)
      TM(Star)
      TM(DoubleStar)
      TM(FSlash)
      TM(BSlash)

      TM(Eq)
      TM(EqEq)
      TM(BangEq)
      TM(LessThan)
      TM(GreaterThan)
      TM(LessThanOrEq)
      TM(GreaterThanOrEq)
      TM(Not)
      TM(And)
      TM(Or)

      TM(BitwiseAnd)
      TM(BitwiseOr)
      TM(BitwiseNot)
      TM(BitWiseLeftShift)
      TM(BitWiseRightShift)

      TM(Dot)
      TM(SemiColon)
      TM(Colon)

      TM(IntLit)
      TM(FloatLit)
      TM(ComplexLit)
      TM(StringLit)
      TM(CharLit)
      TM(Identifier)
      TM(SpicialIdentifier)

      TM(As)

      TM(Mut)
      TM(Let)

      TM(Func)
      TM(Return)
      TM(InlineFunc)

      TM(If)
      TM(Then)
      TM(Unless)
      TM(Else)
      TM(Is)
      TM(For)

      TM(Class)
      TM(FinalClass)

      TM(Try)
      TM(Catch)

    default:
      return "Unknown";
    }
  }
};

template <> struct std::hash<Token> {
  std::size_t operator()(const Token &token) const noexcept {
    std::size_t h1 = std::hash<std::string_view>{}(token.lexem);
    return h1;
  }
};

typedef std::vector<Token> TokenList;

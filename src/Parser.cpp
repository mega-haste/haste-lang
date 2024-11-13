#include "Parser.hpp"
#include "AST/Expressions.hpp"
#include "tokens.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

using namespace AST;

ParserError::ParserError(Token token, const char *message) : token(token), message(message)
{
}

Parser::Parser(TokenList tokens) : m_tokens(tokens)
{
}

TranslationUnit Parser::parse()
{
    TranslationUnit tu;

    while (!is_at_end())
    {
        static_declaration(tu);
    }

    return tu;
}

const bool Parser::match(std::initializer_list<TokenType> types)
{
    for (TokenType type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

const bool Parser::check(TokenType type) const
{
    return is_at_end() ? false : peek() == type;
}

const Token &Parser::consume(TokenType type, const char *message)
{
    if (check(type))
        return advance();

    throw error(peek(), message);
}

const bool Parser::is_at_end(void) const
{
    return peek() == TokenType::_EOF;
}

const Token &Parser::consume_semi_colon(const char *message)
{
    return consume(TokenType::SemiColon, message);
}

const Token &Parser::consume_identifier(const char *message)
{
    if (check(TokenType::Identifier) || check(TokenType::SpicialIdentifier))
        return advance();

    throw error(peek(), message);
}

const Token &Parser::peek(void) const
{
    return m_tokens.at(m_current);
}
const Token &Parser::peek(int ahead) const
{
    return m_tokens.at(m_current + ahead);
}

const Token &Parser::previous(void) const
{
    return m_tokens.at(m_current - 1);
}

const Token &Parser::advance(void)
{
    if (!is_at_end())
        m_current++;
    return previous();
}

ParserError Parser::error(Token token, const char *message)
{
    return ParserError(token, message);
}

void Parser::synchronize(void)
{
    advance();

    advance();

    while (!is_at_end())
    {
        if (previous().type == TokenType::SemiColon)
            return;

        switch (peek().type)
        {
        case TokenType::FinalClass:
        case TokenType::Class:
        case TokenType::Func:
        case TokenType::InlineFunc:
        case TokenType::Let:
        case TokenType::For:
        case TokenType::While:
        case TokenType::Loop:
        case TokenType::If:
        case TokenType::Unless:
        case TokenType::Return:
            return;
        default:
            break;
        }

        advance();
    }
}

void Parser::static_synchronize(void)
{
    advance();

    while (!is_at_end())
    {
        switch (peek().type)
        {
        case TokenType::Func:
        case TokenType::InlineFunc:
            return;
        default:
            break;
        }

        advance();
    }
}

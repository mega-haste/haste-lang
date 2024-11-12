
#include "AST/Statments.hpp"
#include "Parser.hpp"

#include <optional>

Statement Parser::let(void) {
    bool mut = match({TokenType::Mut});
    const Token &name = consume_identifier("Expected identifier of let statement.");
    std::optional<Type> type = match({TokenType::Colon}) ? std::optional{parse_type()} : std::nullopt;
    std::optional<Expression> value = match({TokenType::Eq}) ? std::optional{expression()} : std::nullopt;

    consume_semi_colon("Expected ';' at the end of let statement.");
    return std::make_unique<LetDef>(
        name,
        std::move(value),
        std::move(type),
        mut
    );
}
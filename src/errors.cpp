#include "errors.h"
#include <fmt/core.h>

#define fmtln(line) "(Line " + std::to_string(line) + ")"
#define ERR(...) throw std::runtime_error(fmt::format(__VA_ARGS__))

void errors::lexer::unrecognized_token(size_t line, char tok)
{
    ERR("{} Error: Unrecognized token '{}'.", fmtln(line), tok);
}

void errors::parser::unexpected_token(const Token &tok, TokenType expected)
{
    ERR("{} Error: Unexpected token '{}'; Expected '{}'.",
        fmtln(tok.line), tok.value, Token::tt2str(expected));
}


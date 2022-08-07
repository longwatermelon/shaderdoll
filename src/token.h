#pragma once
#include <string>

enum class TokenType
{
    ID,
    FLOAT,
    SEMI,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    EQUAL,
    COMMA,
    PERIOD,
    BINOP,
    EOF_
};

enum class BinopToken
{
    PLUS,
    MINUS,
    MUL,
    DIV,
    LESS,
    GREATER
};

struct Token
{
    Token() = default;

    Token(TokenType type, std::string value, size_t line)
        : type(type), value(value), line(line) {}

    Token(BinopToken type, std::string value, size_t line)
        : type(TokenType::BINOP), value(value), line(line), binop_type(type) {}

    TokenType type = TokenType::ID;
    std::string value;
    size_t line;

    BinopToken binop_type = BinopToken::PLUS;
};


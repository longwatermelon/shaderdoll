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
    LESS
};

struct Token
{
    Token() = default;

    Token(TokenType type, std::string value)
        : type(type), value(value) {}

    Token(BinopToken type, std::string value)
        : type(TokenType::BINOP), value(value), binop_type(type) {}

    TokenType type = TokenType::ID;
    std::string value;

    BinopToken binop_type = BinopToken::PLUS;
};


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
    EOF_
};

struct Token
{
    Token(TokenType type, std::string value)
        : type(type), value(value) {}

    TokenType type;
    std::string value;
};


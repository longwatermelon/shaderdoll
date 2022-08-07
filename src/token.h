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

    static std::string tt2str(TokenType type)
    {
        switch (type)
        {
        case TokenType::ID: return "id";
        case TokenType::FLOAT: return "float";
        case TokenType::SEMI: return ";";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::EQUAL: return "=";
        case TokenType::COMMA: return ",";
        case TokenType::PERIOD: return ".";
        case TokenType::BINOP: return "binop";
        case TokenType::EOF_: return "EOF";
        }

        return "";
    }

    TokenType type = TokenType::ID;
    std::string value;
    size_t line;

    BinopToken binop_type = BinopToken::PLUS;
};


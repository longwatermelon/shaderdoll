#include "lexer.h"
#include <sstream>
#include <exception>
#include <iostream>
#include <fmt/core.h>

Lexer::Lexer(const std::string &contents)
    : m_contents(contents)
{
    m_ch = contents[0];
}

Lexer::~Lexer()
{
}

void Lexer::advance()
{
    if (m_index < m_contents.size())
        m_ch = m_contents[++m_index];
}

std::string Lexer::collect_float()
{
    std::string res;

    while (std::isdigit(m_ch) || m_ch == '.')
    {
        res += m_ch;
        advance();
    }

    return res;
}

std::string Lexer::collect_id()
{
    std::string res;

    while (std::isalnum(m_ch))
    {
        res += m_ch;
        advance();
    }

    return res;
}

Token Lexer::next_token()
{
    while (m_index < m_contents.size() - 1)
    {
        while (std::isspace(m_ch) && m_ch != '\n')
            advance();

        if (std::isdigit(m_ch))
            return Token(TokenType::FLOAT, collect_float());

        if (std::isalnum(m_ch))
            return Token(TokenType::ID, collect_id());

        auto adv = [&](TokenType type, std::string c) {
            advance();
            return Token(type, c);
        };

        auto advb = [&](BinopToken type, std::string c) {
            advance();
            return Token(type, c);
        };

        switch (m_ch)
        {
        case ';': return adv(TokenType::SEMI, ";");
        case '(': return adv(TokenType::LPAREN, "(");
        case ')': return adv(TokenType::RPAREN, ")");
        case '{': return adv(TokenType::LBRACE, "{");
        case '}': return adv(TokenType::RBRACE, "}");
        case '=': return adv(TokenType::EQUAL, "=");
        case ',': return adv(TokenType::COMMA, ",");
        case '.': return adv(TokenType::PERIOD, ".");

        case '+': return advb(BinopToken::PLUS, "+");
        case '-': return advb(BinopToken::MINUS, "-");
        case '*': return advb(BinopToken::MUL, "*");
        case '/': return advb(BinopToken::DIV, "/");
        case '<': return advb(BinopToken::LESS, "<");
        case '>': return advb(BinopToken::GREATER, ">");

        case '\n':
            ++m_line;
            advance();
            break;
        default:
        {
            throw std::runtime_error(fmt::format("[Lexer::next_token] (Line {}) Error: Token {} is not recognized.", m_line, m_ch));
        } break;
        }
    }

    return Token(TokenType::EOF_, "");
}


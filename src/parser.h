#pragma once
#include "token.h"
#include "node.h"
#include "lexer.h"

class Parser
{
public:
    Parser(const std::string &contents);
    ~Parser();

    void expect(TokenType type);

    std::unique_ptr<Node> parse();
    std::unique_ptr<Node> parse_expr();

    std::unique_ptr<Node> parse_float();
    std::unique_ptr<Node> parse_id();
    std::unique_ptr<Node> parse_ctor();

    std::unique_ptr<Node> parse_var();
    std::unique_ptr<Node> parse_vardef();
    std::unique_ptr<Node> parse_assign();

    std::unique_ptr<Node> parse_fcall();
    std::unique_ptr<Node> parse_fdef(NodeType type, const std::string &name);

    std::unique_ptr<Node> parse_binop(std::unique_ptr<Node> left);

private:
    Token m_curr, m_prev;
    Lexer m_lexer;

    std::unique_ptr<Node> m_prev_node;
};


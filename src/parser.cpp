#include "parser.h"
#include <exception>
#include <sstream>
#include <fmt/core.h>

Parser::Parser(const std::string &contents)
    : m_lexer(contents)
{
    m_curr = m_lexer.next_token();
}

Parser::~Parser()
{
}

void Parser::expect(TokenType type)
{
    if (m_curr.type == type)
    {
        m_prev = m_curr;
        m_curr = m_lexer.next_token();
    }
    else
    {
        throw std::runtime_error(fmt::format("[Parser::expect] Error: Unexpected token '{}' of type {}; Expected {}.",
                                 m_curr.value, (int)m_curr.type, (int)type));
    }
}

std::unique_ptr<Node> Parser::parse()
{
    std::unique_ptr<Node> root = std::make_unique<Node>(NodeType::COMPOUND);
    root->comp_values.emplace_back(parse_expr());

    if (root->comp_values[0])
    {
        while (m_curr.type != TokenType::EOF_)
        {
            expect(TokenType::SEMI);

            std::unique_ptr<Node> expr = parse_expr();
            if (!expr) break;

            root->comp_values.emplace_back(std::move(expr));
        }
    }
    else
    {
        root->comp_values[0] = std::make_unique<Node>(NodeType::NOOP);
    }

    return root;
}

std::unique_ptr<Node> Parser::parse_expr()
{
    bool found = true;

    switch (m_curr.type)
    {
    case TokenType::FLOAT: m_prev_node = parse_float(); break;
    case TokenType::ID: m_prev_node = parse_id(); break;
    case TokenType::LPAREN:
        expect(TokenType::LPAREN);
        m_prev_node = parse_expr();

        if (m_prev_node->type == NodeType::BINOP)
            m_prev_node->op_priority = true;

        expect(TokenType::RPAREN);
        break;
    default: found = false; break;
    }

    if (found)
    {
        if (m_curr.type == TokenType::BINOP)
        {
            std::unique_ptr<Node> prev = parse_binop(std::move(m_prev_node));
            m_prev_node = std::move(prev);
        }
    }
    else
    {
        m_prev_node.reset(0);
    }

    return std::move(m_prev_node);
}

std::unique_ptr<Node> Parser::parse_float()
{
    std::unique_ptr<Node> node = std::make_unique<Node>(NodeType::FLOAT);
    node->float_value = std::atof(m_curr.value.c_str());
    expect(TokenType::FLOAT);

    return node;
}

std::unique_ptr<Node> Parser::parse_id()
{
    if (m_curr.value == "return") return parse_return();
    if (Node::str2dtype(m_curr.value) != NodeType::NOOP) return parse_vardef();

    return parse_var();
}

std::unique_ptr<Node> Parser::parse_ctor()
{
    std::unique_ptr<Node> node = std::make_unique<Node>(NodeType::CONSTRUCTOR);

    // On lparen
    node->ctor_type = Node::str2dtype(m_prev.value);
    expect(TokenType::LPAREN);

    while (m_curr.type != TokenType::RPAREN)
    {
        node->ctor_args.emplace_back(parse_expr());

        if (m_curr.type != TokenType::RPAREN)
            expect(TokenType::COMMA);
    }

    expect(TokenType::RPAREN);
    return node;
}

std::unique_ptr<Node> Parser::parse_var()
{
    std::string id = m_curr.value;
    expect(TokenType::ID);

    if (m_curr.type == TokenType::LPAREN) return parse_fcall();
    if (m_curr.type == TokenType::EQUAL) return parse_assign();

    std::unique_ptr<Node> n = std::make_unique<Node>(NodeType::VAR);
    n->var_name = id;

    if (m_curr.type == TokenType::PERIOD)
    {
        expect(TokenType::PERIOD);
        n->var_memb_access = m_curr.value[0];
        expect(TokenType::ID);
    }

    return n;
}

std::unique_ptr<Node> Parser::parse_vardef()
{
    NodeType type = Node::str2dtype(m_curr.value);
    expect(TokenType::ID);

    if (m_curr.type == TokenType::LPAREN)
        return parse_ctor();

    std::string name = m_curr.value;
    expect(TokenType::ID);

    if (m_curr.type == TokenType::LPAREN)
        return parse_fdef(type, name);

    std::unique_ptr<Node> node = std::make_unique<Node>(NodeType::VARDEF);
    node->vardef_name = name;
    node->vardef_type = type;

    if (m_curr.type == TokenType::EQUAL)
    {
        expect(TokenType::EQUAL);

        std::unique_ptr<Node> value = parse_expr();
        node->vardef_value = std::move(value);
    }
    else
    {
        node->vardef_value = std::make_unique<Node>(type);
    }

    return node;
}

std::unique_ptr<Node> Parser::parse_assign()
{
    std::unique_ptr<Node> node = std::make_unique<Node>(NodeType::ASSIGN);
    node->assign_l = std::make_unique<Node>(NodeType::VAR);
    node->assign_l->var_name = m_prev.value;

    expect(TokenType::EQUAL);

    node->assign_r = parse_expr();

    return node;
}

std::unique_ptr<Node> Parser::parse_fcall()
{
    std::unique_ptr<Node> node = std::make_unique<Node>(NodeType::FCALL);
    node->fcall_name = m_prev.value;

    expect(TokenType::LPAREN);

    while (m_curr.type != TokenType::RPAREN)
    {
        std::unique_ptr<Node> expr = parse_expr();
        node->fcall_args.emplace_back(std::move(expr));

        if (m_curr.type != TokenType::RPAREN)
            expect(TokenType::COMMA);
    }

    expect(TokenType::RPAREN);
    return node;
}

std::unique_ptr<Node> Parser::parse_fdef(NodeType type, const std::string &name)
{
    std::unique_ptr<Node> node = std::make_unique<Node>(NodeType::FDEF);
    node->fdef_rtype = type;
    node->fdef_name = name;

    expect(TokenType::LPAREN);

    while (m_curr.type != TokenType::RPAREN)
    {
        std::unique_ptr<Node> param = std::make_unique<Node>(NodeType::PARAM);

        param->param_type = Node::str2dtype(m_curr.value);
        expect(TokenType::ID);

        param->param_name = m_curr.value;
        expect(TokenType::ID);

        node->fdef_params.emplace_back(std::move(param));

        if (m_curr.type != TokenType::RPAREN)
            expect(TokenType::COMMA);
    }

    expect(TokenType::RPAREN);

    expect(TokenType::LBRACE);
    node->fdef_body = parse();
    expect(TokenType::RBRACE);

    return node;
}

std::unique_ptr<Node> Parser::parse_return()
{
    expect(TokenType::ID);

    std::unique_ptr<Node> n = std::make_unique<Node>(NodeType::RETURN);
    n->ret_value = parse_expr();

    return n;
}

std::unique_ptr<Node> Parser::parse_binop(std::unique_ptr<Node> left)
{
    std::unique_ptr<Node> n = std::make_unique<Node>(NodeType::BINOP);

    switch (m_curr.value[0])
    {
    case '+': n->op = BinopType::ADD; break;
    case '-': n->op = BinopType::SUB; break;
    case '*': n->op = BinopType::MUL; break;
    case '/': n->op = BinopType::DIV; break;
    }

    expect(TokenType::BINOP);

    n->op_l = std::move(left);
    std::unique_ptr<Node> parent = parse_expr();

    if (parent->type == NodeType::BINOP && !parent->op_priority)
    {
        std::unique_ptr<Node> *bl_parent = &parent;
        std::unique_ptr<Node> *bl = &parent->op_l;

        while (*bl && (*bl)->op_l->type == NodeType::BINOP)
        {
            bl_parent = bl;
            bl = &(*bl)->op_l;
        }

        /* n->op_r = std::move((*bl)->op_l); */
        /* (*bl)->op_l = std::move(n); */
        n->op_r = std::move(*bl);
        (*bl_parent)->op_l = std::move(n);
        return parent;
    }
    else
    {
        n->op_r = std::move(parent);
        return n;
    }
}


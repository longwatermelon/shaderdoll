#include "parser.h"
#include <exception>
#include <sstream>

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
        std::stringstream ss;
        ss << "[Parser::expect] Error: Unexpected token '" << m_curr.value << "' of type "
           << (int)m_curr.type << "; Expected " << (int)type << ".\n";
        throw std::runtime_error(ss.str());
    }
}

std::unique_ptr<Node> Parser::parse()
{
    std::unique_ptr<Node> root = std::make_unique<Node>(NodeType::COMPOUND);
    root->comp_values.emplace_back(parse_expr());

    while (m_curr.type != TokenType::EOF_)
    {
        expect(TokenType::SEMI);

        std::unique_ptr<Node> expr = parse_expr();
        if (!expr) break;

        root->comp_values.emplace_back(std::move(expr));
    }

    return root;
}

std::unique_ptr<Node> Parser::parse_expr()
{
    switch (m_curr.type)
    {
    case TokenType::FLOAT: return parse_float();
    case TokenType::ID: return parse_id();
    default: return nullptr;
    }
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
    if (Node::str2dtype(m_curr.value) != NodeType::NOOP)
        return parse_vardef();

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
    n->var_name = m_curr.value;

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


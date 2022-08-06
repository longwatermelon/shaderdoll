#include "visitor.h"
#include "builtin.h"
#include <exception>
#include <sstream>
#include <fmt/core.h>

static std::unique_ptr<Node> g_fret;

Visitor::Visitor()
{
}

Visitor::~Visitor()
{
}

Node *Visitor::visit(Node *node)
{
    if (!node) return nullptr;

    switch (node->type)
    {
    case NodeType::NOOP:
    case NodeType::FLOAT:
    case NodeType::VOID:
        return node;
    case NodeType::VEC: return visit_vec(node);
    case NodeType::CONSTRUCTOR: return visit_ctor(node);
    case NodeType::COMPOUND: return visit_compound(node);
    case NodeType::VARDEF: return visit_vardef(node);
    case NodeType::VAR: return visit_var(node);
    case NodeType::ASSIGN: return visit_assign(node);
    case NodeType::FCALL: return visit_fcall(node);
    case NodeType::FDEF: return visit_fdef(node);
    case NodeType::PARAM: return visit_param(node);
    case NodeType::BINOP: return visit_binop(node);
    case NodeType::RETURN:
        g_fret = visit(node->ret_value.get())->copy();
        return g_fret.get();
    }

    throw std::runtime_error(fmt::format("[Visitor::visit] Error: Uncaught statement of type {}.", (int)node->type));
}

Node *Visitor::visit_compound(Node *node)
{
    for (auto &v : node->comp_values)
        visit(v.get());

    return node;
}

Node *Visitor::visit_vardef(Node *n)
{
    Node *value = visit(n->vardef_value.get());
    n->vardef_value = value->copy();

    m_scope.add_vardef(n);
    return n->vardef_value.get();
}

Node *Visitor::visit_var(Node *n)
{
    Node *def = m_scope.find_vardef(n->var_name);

    if (def->vardef_type == NodeType::VEC)
    {
        switch (n->var_memb_access)
        {
        case 'x': return visit(def->vardef_value->vec_values[0].get());
        case 'y': return visit(def->vardef_value->vec_values[1].get());
        case 'z': return visit(def->vardef_value->vec_values[2].get());
        }
    }

    return visit(def);
}

Node *Visitor::visit_assign(Node *n)
{
    // Directly access assign_l because it's guaranteed to be NODE_VAR
    Node *def = m_scope.find_vardef(n->assign_l->var_name);
    Node *right = visit(n->assign_r.get());

    if (def->vardef_type != right->type)
    {
        throw std::runtime_error(fmt::format("[Visitor::visit_assign] Error: Mismatched types when assigning to '"
                                             "{}': {} and {}.", def->vardef_name, (int)def->vardef_type, (int)right->type));
    }

    def->vardef_value = right->copy();
    return n;
}

Node *Visitor::visit_fcall(Node *n)
{
    g_fret = std::make_unique<Node>(NodeType::VOID);

    for (auto &arg : n->fcall_args)
        arg = visit(arg.get())->copy();

    if (n->fcall_name == "print") return builtin::print(n);

    Node *def = m_scope.find_fdef(n->fcall_name);
    m_scope.push_layer();
    visit(def->fdef_body.get());

    n->fcall_ret = g_fret->copy();
    m_scope.pop_layer();

    return n->fcall_ret.get();
}

Node *Visitor::visit_fdef(Node *n)
{
    m_scope.push_layer();
    m_scope.add_fdef(n);
    m_scope.pop_layer();
    return n;
}

Node *Visitor::visit_ctor(Node *n)
{
    for (auto &e : n->ctor_args)
    {
        Node *value = visit(e.get());
        e = value->copy();
    }

    n->ctor_res = builtin::construct(n);
    return n->ctor_res.get();
}

Node *Visitor::visit_vec(Node *n)
{
    for (auto &v : n->vec_values)
        v = visit(v.get())->copy();

    return n;
}

Node *Visitor::visit_param(Node *n)
{
    Node *def = m_scope.find_vardef(n->param_name);
    return visit(def->vardef_value.get());
}

#define BINOP_EXEC(a, b, op, res) { \
    if (op == BinopType::ADD) res = a + b; \
    if (op == BinopType::SUB) res = a - b; \
    if (op == BinopType::MUL) res = a * b; \
    if (op == BinopType::DIV) res = a / b; \
}

Node *Visitor::visit_binop(Node *n)
{
    Node *l = visit(n->op_l.get());
    Node *r = visit(n->op_r.get());

    if (l->type != r->type)
    {
        throw std::runtime_error(fmt::format("[Visitor::visit_binop] Error: Incompatible types."));
    }

    n->op_res = std::make_unique<Node>(l->type);

    switch (l->type)
    {
    case NodeType::FLOAT:
        BINOP_EXEC(l->float_value, r->float_value, n->op, n->op_res->float_value);
        break;
    default:
        throw std::runtime_error(fmt::format("[Visitor::visit_binop] Error: Performing operator on invalid type."));
    }

    return n->op_res.get();
}

void Visitor::add_var(std::unique_ptr<Node> vardef)
{
    m_inputs.emplace_back(std::move(vardef));
    m_scope.add_vardef(m_inputs.back().get());
}


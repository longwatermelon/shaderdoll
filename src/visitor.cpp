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
    case NodeType::BOOL:
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
    case NodeType::IF: return visit_if(node);
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

    Node *builtin_call = builtin::call(n);
    if (builtin_call) return builtin_call;

    Node *def = m_scope.find_fdef(n->fcall_name);
    m_scope.push_layer();

    for (size_t i = 0; i < n->fcall_args.size(); ++i)
    {
        std::unique_ptr<Node> vardef = std::make_unique<Node>(NodeType::VARDEF);
        vardef->vardef_type = def->fdef_params[i]->param_type;
        vardef->vardef_name = def->fdef_params[i]->param_name;
        vardef->vardef_value = n->fcall_args[i]->copy();

        m_scope.add_param(std::move(vardef));
    }

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
    if (op == BinopType::ADD) res->float_value = a + b; \
    if (op == BinopType::SUB) res->float_value = a - b; \
    if (op == BinopType::MUL) res->float_value = a * b; \
    if (op == BinopType::DIV) res->float_value = a / b; \
    if (op == BinopType::LESS) \
    { \
        res->type = NodeType::BOOL; \
        res->bool_value = a < b; \
    } \
    \
    if (op == BinopType::GREATER) \
    { \
        res->type = NodeType::BOOL; \
        res->bool_value = a > b; \
    } \
}

#define BINOP_VEC_EXEC(a, b, op, res) {\
    for (size_t i = 0; i < a->vec_values.size(); ++i) \
    { \
        res->vec_values[i] = std::make_unique<Node>(NodeType::FLOAT); \
        res->vec_values[i]->float_value = a->vec_values[i]->float_value op b->vec_values[i]->float_value; \
    } \
}

#define BINOP_VEC(a, b, op, res) {\
    res->vec_values.resize(a->vec_values.size()); \
    if (op == BinopType::ADD) BINOP_VEC_EXEC(a, b, +, res); \
    if (op == BinopType::SUB) BINOP_VEC_EXEC(a, b, -, res); \
    if (op == BinopType::MUL) BINOP_VEC_EXEC(a, b, *, res); \
    if (op == BinopType::DIV) BINOP_VEC_EXEC(a, b, /, res); \
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
        BINOP_EXEC(l->float_value, r->float_value, n->op, n->op_res);
        break;
    case NodeType::VEC:
        BINOP_VEC(l, r, n->op, n->op_res);
        break;
    default:
        throw std::runtime_error(fmt::format("[Visitor::visit_binop] Error: Performing operator on invalid type."));
    }

    return n->op_res.get();
}

Node *Visitor::visit_if(Node *n)
{
    n->if_cond = visit(n->if_cond.get())->copy();

    if (n->if_cond->bool_value)
        return visit(n->if_body.get());

    return nullptr;
}

void Visitor::add_var(std::unique_ptr<Node> vardef)
{
    m_inputs.emplace_back(std::move(vardef));
    m_scope.add_vardef(m_inputs.back().get());
}


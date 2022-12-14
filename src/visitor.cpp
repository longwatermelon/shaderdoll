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

    throw std::runtime_error(fmt::format("[Visitor::visit] (Line {}) Error: Uncaught statement of type {}.",
                             node->line, (int)node->type));
}

Node *Visitor::visit_compound(Node *node)
{
    for (auto &v : node->comp_values)
        visit(v.get());

    return node;
}

Node *Visitor::visit_vardef(Node *n)
{
    n->vardef_value_res = visit(n->vardef_value.get())->copy();

    m_scope.add_vardef(n);
    return n->vardef_value_res.get();
}

Node *Visitor::visit_var(Node *n)
{
    Node *def = m_scope.find_vardef(n->var_name, true);

    if (def->vardef_type == NodeType::VEC)
    {
        switch (n->var_memb_access)
        {
        case 'x': return visit(def->vardef_value_res->vec_values[0].get());
        case 'y': return visit(def->vardef_value_res->vec_values[1].get());
        case 'z': return visit(def->vardef_value_res->vec_values[2].get());
        }
    }

    return visit(def->vardef_value_res.get());
}

Node *Visitor::visit_assign(Node *n)
{
    // Directly access assign_l because it's guaranteed to be NODE_VAR
    Node *def = m_scope.find_vardef(n->assign_l->var_name, true);
    Node *right = visit(n->assign_r.get());

    if (def->vardef_type != right->type)
    {
        throw std::runtime_error(fmt::format("[Visitor::visit_assign] (Line {}) Error: Mismatched types when assigning to '"
                                             "{}': {} and {}.", right->line, def->vardef_name, (int)def->vardef_type, (int)right->type));
    }

    def->vardef_value_res = right->copy();
    return n;
}

Node *Visitor::visit_fcall(Node *n)
{
    g_fret = std::make_unique<Node>(NodeType::VOID);

    n->fcall_args_res.resize(n->fcall_args.size());
    for (size_t i = 0; i < n->fcall_args.size(); ++i)
        n->fcall_args_res[i] = visit(n->fcall_args[i].get())->copy();

    Node *builtin_call = builtin::call(n);
    if (builtin_call) return builtin_call;

    Node *def = m_scope.find_fdef(n->fcall_name, true);
    m_scope.push_layer();

    for (size_t i = 0; i < n->fcall_args_res.size(); ++i)
    {
        std::unique_ptr<Node> vardef = std::make_unique<Node>(NodeType::VARDEF);
        vardef->vardef_type = def->fdef_params[i]->param_type;
        vardef->vardef_name = def->fdef_params[i]->param_name;
        vardef->vardef_value = n->fcall_args_res[i]->copy();
        vardef->vardef_value_res = vardef->vardef_value->copy();

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
    n->ctor_args_res.resize(n->ctor_args.size());
    for (size_t i = 0; i < n->ctor_args.size(); ++i)
        n->ctor_args_res[i] = visit(n->ctor_args[i].get())->copy();

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
    Node *def = m_scope.find_vardef(n->param_name, true);
    return visit(def->vardef_value_res.get());
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
        throw std::runtime_error(fmt::format(
            "[Visitor::visit_binop] (Line {}) Error: Incompatible types {} and {}.",
            l->line, (int)l->type, (int)r->type));
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
        throw std::runtime_error(fmt::format(
            "[Visitor::visit_binop] (Line {}) Error: Performing operator on invalid type {}.",
            n->line, (int)l->type));
    }

    return n->op_res.get();
}

Node *Visitor::visit_if(Node *n)
{
    n->if_cond_res = visit(n->if_cond.get())->copy();

    if (n->if_cond_res->bool_value)
        return visit(n->if_body.get());
    else
    {
        if (n->if_else_body)
            return visit(n->if_else_body.get());
    }

    return nullptr;
}

void Visitor::add_input(std::unique_ptr<Node> vardef)
{
    m_inputs.emplace_back(std::move(vardef));
    m_scope.add_global_vardef(m_inputs.back().get());
}


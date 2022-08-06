#include "visitor.h"
#include "builtin.h"
#include <exception>
#include <sstream>

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
    }

    throw std::runtime_error("[Visitor::visit] Error: Uncaught statement of type " +
                             std::to_string((int)node->type) + ".\n");
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
    return n;
}

Node *Visitor::visit_var(Node *n)
{
    Node *def = m_scope.find_vardef(n->var_name);
    return visit(def);
}

Node *Visitor::visit_assign(Node *n)
{
    // Directly access assign_l because it's guaranteed to be NODE_VAR
    Node *def = m_scope.find_vardef(n->assign_l->var_name);
    Node *right = visit(n->assign_r.get());

    if (def->vardef_type != right->type)
    {
        std::stringstream ss;
        ss << "[Visitor::visit_assign] Error: Mismatched types when assigning to '" <<
              def->vardef_name << "': " << (int)def->vardef_type << " and " << (int)right->type <<
              ".\n";
        throw std::runtime_error(ss.str());
    }

    def->vardef_value = right->copy();
    return n;
}

Node *Visitor::visit_fcall(Node *n)
{
    Node *def = m_scope.find_fdef(n->fcall_name);
    return visit(def->fdef_body.get());
}

Node *Visitor::visit_fdef(Node *n)
{
    m_scope.add_fdef(n);
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
}

Node *Visitor::visit_binop(Node *n)
{
    Node *l = visit(n->op_l.get());
    Node *r = visit(n->op_r.get());

    if (l->type != r->type)
    {
        throw std::runtime_error("[Visitor::visit_binop] Error: Incompatible types.");
    }

    n->op_res = std::make_unique<Node>(l->type);

    switch (l->type)
    {
    case NodeType::FLOAT:
        BINOP_EXEC(l->float_value, r->float_value, n->op, n->op_res->float_value);
        break;
    default:
        throw std::runtime_error("[Visitor::visit_binop] Error: Performing operator on invalid type.");
    }

    return n->op_res.get();
}


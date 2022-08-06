#include "node.h"

Node::Node(NodeType type)
    : type(type)
{
}

std::unique_ptr<Node> Node::copy()
{
    std::unique_ptr<Node> ret = std::make_unique<Node>(type);

    switch (type)
    {
    case NodeType::FLOAT:
        ret->float_value = float_value;
        break;
    case NodeType::VOID:
    case NodeType::NOOP:
        break;

    case NodeType::CONSTRUCTOR:
        for (size_t i = 0; i < ctor_args.size(); ++i)
            ret->ctor_args[i] = ctor_args[i]->copy();

        ret->ctor_type = ctor_type;
        break;

    case NodeType::ASSIGN:
        ret->assign_l = assign_l->copy();
        ret->assign_r = assign_r->copy();
        break;
    case NodeType::COMPOUND:
        for (auto &v : comp_values)
            ret->comp_values.emplace_back(v->copy());

        break;
    case NodeType::VAR:
        ret->var_name = var_name;
        break;
    case NodeType::VARDEF:
        ret->vardef_type = vardef_type;
        ret->vardef_name = vardef_name;
        ret->vardef_value = vardef_value->copy();
        break;

    case NodeType::FCALL:
        ret->fcall_name = fcall_name;
        for (auto &v : fcall_args)
            ret->fcall_args.emplace_back(v->copy());

        break;
    case NodeType::FDEF:
        ret->fdef_rtype = fdef_rtype;
        ret->fdef_name = fdef_name;
        ret->fdef_body = fdef_body->copy();
        for (auto &v : fdef_params)
            ret->fdef_params.emplace_back(v->copy());

        break;

    case NodeType::PARAM:
        ret->param_type = param_type;
        ret->param_name = param_name;
        break;
    }

    return ret;
}

std::string Node::dtype2str(NodeType type)
{
    switch (type)
    {
    case NodeType::FLOAT: return "float";
    case NodeType::VOID: return "void";
    default: return "";
    }
}

NodeType Node::str2dtype(const std::string &s)
{
    if (s == "float") return NodeType::FLOAT;
    if (s == "void") return NodeType::VOID;

    return NodeType::NOOP;
}


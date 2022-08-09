#include "node.h"

Node::Node(NodeType type)
    : type(type)
{
}

std::unique_ptr<Node> Node::copy()
{
    std::unique_ptr<Node> ret = std::make_unique<Node>(type);
    ret->line = line;

    switch (type)
    {
    case NodeType::FLOAT:
        ret->float_value = float_value;
        break;
    case NodeType::BOOL:
        ret->bool_value = bool_value;
        break;
    case NodeType::VEC:
        ret->vec_values.resize(vec_values.size());
        for (size_t i = 0; i < vec_values.size(); ++i)
            ret->vec_values[i] = vec_values[i]->copy();

        break;
    case NodeType::VOID:
    case NodeType::NOOP:
        break;

    case NodeType::CONSTRUCTOR:
        ret->ctor_args.resize(ctor_args.size());
        ret->ctor_args_res.resize(ctor_args_res.size());
        for (size_t i = 0; i < ctor_args.size(); ++i)
            ret->ctor_args[i] = ctor_args[i]->copy();

        for (size_t i = 0; i < ctor_args_res.size(); ++i)
            ret->ctor_args_res[i] = ctor_args_res[i]->copy();

        ret->ctor_type = ctor_type;
        ret->ctor_res = ctor_res ? ctor_res->copy() : nullptr;
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
        ret->var_memb_access = var_memb_access;
        break;
    case NodeType::VARDEF:
        ret->vardef_type = vardef_type;
        ret->vardef_name = vardef_name;
        ret->vardef_value = vardef_value->copy();
        ret->vardef_value_res = vardef_value_res ? vardef_value_res->copy() : nullptr;
        break;

    case NodeType::FCALL:
        ret->fcall_name = fcall_name;
        ret->fcall_ret = fcall_ret ? fcall_ret->copy() : nullptr;

        ret->fcall_args_res.resize(fcall_args_res.size());
        for (auto &v : fcall_args)
            ret->fcall_args.emplace_back(v->copy());

        for (size_t i = 0; i < ret->fcall_args_res.size(); ++i)
            ret->fcall_args_res[i] = fcall_args_res[i]->copy();

        break;
    case NodeType::FDEF:
        ret->fdef_rtype = fdef_rtype;
        ret->fdef_name = fdef_name;
        ret->fdef_body = fdef_body->copy();
        for (auto &v : fdef_params)
            ret->fdef_params.emplace_back(v->copy());

        break;
    case NodeType::RETURN:
        ret->ret_value = ret_value->copy();
        break;

    case NodeType::PARAM:
        ret->param_type = param_type;
        ret->param_name = param_name;
        break;

    case NodeType::BINOP:
        ret->op = op;
        ret->op_l = op_l->copy();
        ret->op_r = op_r->copy();
        ret->op_res = op_res ? op_res->copy() : nullptr;
        ret->op_priority = op_priority;
        break;
    case NodeType::IF:
        ret->if_cond = if_cond->copy();
        ret->if_body = if_body->copy();
        ret->if_else_body = if_else_body ? if_else_body->copy() : nullptr;
        ret->if_cond_res = if_cond_res ? if_cond_res->copy() : nullptr;
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
    case NodeType::BOOL: return "bool";
    case NodeType::VEC: return "vec";
    default: return "";
    }
}

NodeType Node::str2dtype(const std::string &s)
{
    if (s == "float") return NodeType::FLOAT;
    if (s == "void") return NodeType::VOID;
    if (s == "bool") return NodeType::BOOL;
    if (s.substr(0, 3) == "vec") return NodeType::VEC;

    return NodeType::NOOP;
}

std::string Node::ntype2str(NodeType type)
{
    switch (type)
    {
    case NodeType::NOOP: return "noop";
    case NodeType::VEC: return "vec";
    case NodeType::BOOL: return "bool";
    case NodeType::VOID: return "void";
    case NodeType::FLOAT: return "float";
    case NodeType::IF: return "if";
    case NodeType::BINOP: return "binop";
    case NodeType::RETURN: return "return";
    case NodeType::ASSIGN: return "assignment";
    case NodeType::COMPOUND: return "compound";
    case NodeType::CONSTRUCTOR: return "constructor";
    case NodeType::FCALL: return "function call";
    case NodeType::FDEF: return "function definition";
    case NodeType::PARAM: return "parameter";
    case NodeType::VAR: return "variable";
    case NodeType::VARDEF: return "variable definition";
    }

    return "";
}


#include "builtin.h"
#include <iostream>
#include <cmath>
#include <map>
#include <functional>

std::unique_ptr<Node> builtin::construct(Node *ctor)
{
    switch (ctor->ctor_type)
    {
    case NodeType::FLOAT: return construct_float(ctor);
    case NodeType::VEC: return construct_vec(ctor);
    default: break;
    }

    return 0;
}

std::unique_ptr<Node> builtin::construct_float(Node *ctor)
{
    std::unique_ptr<Node> n = std::make_unique<Node>(NodeType::FLOAT);
    n->float_value = ctor->ctor_args[0]->float_value;
    return n;
}

std::unique_ptr<Node> builtin::construct_vec(Node *ctor)
{
    std::unique_ptr<Node> n = std::make_unique<Node>(NodeType::VEC);
    n->vec_values.resize(ctor->ctor_args.size());

    for (size_t i = 0; i < n->vec_values.size(); ++i)
        n->vec_values[i] = ctor->ctor_args[i]->copy();

    return n;
}

Node *builtin::call(Node *fcall)
{
    std::map<std::string, std::function<Node*(Node*)>> map{
        { "print", builtin::print },
        { "sqrt", builtin::sqrt },
        { "distance", builtin::distance },
        { "dot", builtin::dot },
        { "normalize", builtin::normalize },
        { "length", builtin::length },

        { "sin", builtin::sin },
        { "cos", builtin::cos },
        { "tan", builtin::tan },
        { "abs", builtin::abs }
    };

    if (map.find(fcall->fcall_name) != map.end())
        return map[fcall->fcall_name](fcall);

    return nullptr;
}

Node *builtin::print(Node *fcall)
{
    for (auto &arg : fcall->fcall_args)
    {
        switch (arg->type)
        {
        case NodeType::FLOAT: std::cout << arg->float_value; break;
        case NodeType::VEC:
            std::cout << "{ ";
            for (size_t i = 0; i < arg->vec_values.size(); ++i)
                std::cout << arg->vec_values[i]->float_value << (i == arg->vec_values.size() - 1 ? "" : ", ");
            std::cout << " }";
            break;
        default:
            throw std::runtime_error("[builtin::print] Error: Can't print this type.");
        }

        std::cout << ' ';
    }

    std::cout << "\n";
    return fcall;
}

Node *builtin::sqrt(Node *fcall)
{
    Node *arg = fcall->fcall_args[0].get();
    fcall->fcall_ret = std::make_unique<Node>(NodeType::FLOAT);
    fcall->fcall_ret->float_value = std::sqrt(arg->float_value);
    return fcall->fcall_ret.get();
}

Node *builtin::distance(Node *fcall)
{
    Node *a = fcall->fcall_args[0].get();
    Node *b = fcall->fcall_args[1].get();

    float res = 0.f;
    for (size_t i = 0; i < a->vec_values.size(); ++i)
        res += std::pow(a->vec_values[i]->float_value - b->vec_values[i]->float_value, 2);

    fcall->fcall_ret = std::make_unique<Node>(NodeType::FLOAT);
    fcall->fcall_ret->float_value = std::sqrt(res);
    return fcall->fcall_ret.get();
}

Node *builtin::dot(Node *fcall)
{
    Node *a = fcall->fcall_args[0].get();
    Node *b = fcall->fcall_args[1].get();

    float res = 0.f;
    for (size_t i = 0; i < a->vec_values.size(); ++i)
        res += a->vec_values[i]->float_value * b->vec_values[i]->float_value;

    fcall->fcall_ret = std::make_unique<Node>(NodeType::FLOAT);
    fcall->fcall_ret->float_value = res;
    return fcall->fcall_ret.get();
}

Node *builtin::normalize(Node *fcall)
{
    Node *arg = fcall->fcall_args[0].get();

    float len = 0.f;
    for (size_t i = 0; i < arg->vec_values.size(); ++i)
        len += std::pow(arg->vec_values[i]->float_value, 2);

    len = std::sqrt(len);
    fcall->fcall_ret = std::make_unique<Node>(NodeType::VEC);
    fcall->fcall_ret->vec_values.resize(arg->vec_values.size());

    for (size_t i = 0; i < arg->vec_values.size(); ++i)
    {
        fcall->fcall_ret->vec_values[i] = std::make_unique<Node>(NodeType::FLOAT);
        fcall->fcall_ret->vec_values[i]->float_value = arg->vec_values[i]->float_value / len;
    }

    return fcall->fcall_ret.get();
}

Node *builtin::length(Node *fcall)
{
    Node *arg = fcall->fcall_args[0].get();

    float len = 0.f;
    for (size_t i = 0; i < arg->vec_values.size(); ++i)
        len += std::pow(arg->vec_values[i]->float_value, 2);

    fcall->fcall_ret = std::make_unique<Node>(NodeType::FLOAT);
    fcall->fcall_ret->float_value = std::sqrt(len);

    return fcall->fcall_ret.get();
}

Node *builtin::sin(Node *fcall)
{
    Node *arg = fcall->fcall_args[0].get();
    fcall->fcall_ret = std::make_unique<Node>(NodeType::FLOAT);
    fcall->fcall_ret->float_value = std::sin(arg->float_value);

    return fcall->fcall_ret.get();
}

Node *builtin::cos(Node *fcall)
{
    Node *arg = fcall->fcall_args[0].get();
    fcall->fcall_ret = std::make_unique<Node>(NodeType::FLOAT);
    fcall->fcall_ret->float_value = std::cos(arg->float_value);

    return fcall->fcall_ret.get();
}

Node *builtin::tan(Node *fcall)
{
    Node *arg = fcall->fcall_args[0].get();
    fcall->fcall_ret = std::make_unique<Node>(NodeType::FLOAT);
    fcall->fcall_ret->float_value = std::tan(arg->float_value);

    return fcall->fcall_ret.get();
}

Node *builtin::abs(Node *fcall)
{
    Node *arg = fcall->fcall_args[0].get();
    fcall->fcall_ret = std::make_unique<Node>(NodeType::FLOAT);
    fcall->fcall_ret->float_value = std::abs(arg->float_value);

    return fcall->fcall_ret.get();
}


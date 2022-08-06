#include "builtin.h"
#include <iostream>

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


#include "builtin.h"

std::unique_ptr<Node> builtin::construct(Node *ctor)
{
    switch (ctor->ctor_type)
    {
    case NodeType::FLOAT: return construct_float(ctor);
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


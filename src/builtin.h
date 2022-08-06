#pragma once
#include "node.h"
#include "visitor.h"

namespace builtin
{
    // All constructors should already be visited by visitor
    std::unique_ptr<Node> construct(Node *ctor);
    std::unique_ptr<Node> construct_float(Node *ctor);
    std::unique_ptr<Node> construct_vec(Node *ctor);
}


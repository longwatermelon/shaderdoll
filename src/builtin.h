#pragma once
#include "node.h"
#include "visitor.h"

namespace builtin
{
    // All constructors should already be visited by visitor
    std::unique_ptr<Node> construct(Node *ctor);
    std::unique_ptr<Node> construct_float(Node *ctor);
    std::unique_ptr<Node> construct_vec(Node *ctor);

    Node *call(Node *fcall);

    Node *print(Node *fcall);
    Node *sqrt(Node *fcall);
    Node *distance(Node *fcall);
    Node *dot(Node *fcall);
    Node *normalize(Node *fcall);
    Node *length(Node *fcall);

    Node *sin(Node *fcall);
    Node *cos(Node *fcall);
    Node *tan(Node *fcall);
    Node *asin(Node *fcall);
    Node *acos(Node *fcall);
    Node *atan(Node *fcall);
    Node *abs(Node *fcall);
    Node *pow(Node *fcall);
}


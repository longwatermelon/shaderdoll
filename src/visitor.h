#pragma once
#include "scope.h"

class Visitor
{
public:
    Visitor();
    ~Visitor();

    Node *visit(Node *node);
    Node *visit_compound(Node *node);

    Node *visit_vardef(Node *n);
    Node *visit_var(Node *n);
    Node *visit_assign(Node *n);

    Node *visit_fcall(Node *n);
    Node *visit_fdef(Node *n);
    Node *visit_ctor(Node *n);

    Node *visit_vec(Node *n);

    Node *visit_param(Node *n);

private:
    Scope m_scope;
};


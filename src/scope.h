#pragma once
#include "node.h"

class Scope
{
public:
    Scope();
    ~Scope();

    void add_vardef(Node *node);
    void add_fdef(Node *node);

    Node *find_vardef(const std::string &name);
    Node *find_fdef(const std::string &name);

private:
    std::vector<Node*> m_vardefs, m_fdefs;
};


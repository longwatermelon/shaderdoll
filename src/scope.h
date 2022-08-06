#pragma once
#include "node.h"

struct ScopeLayer
{
    std::vector<Node*> vardefs;
};

class Scope
{
public:
    Scope();
    ~Scope();

    void add_vardef(Node *node);
    void add_fdef(Node *node);

    Node *find_vardef(const std::string &name);
    Node *find_fdef(const std::string &name);

    void push_layer();
    void pop_layer();

private:
    std::vector<ScopeLayer> m_layers;
    std::vector<Node*> m_fdefs;
};


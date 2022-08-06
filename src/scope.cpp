#include "scope.h"

Scope::Scope()
{
    push_layer();
}

Scope::~Scope()
{
}

void Scope::add_vardef(Node *node)
{
    m_layers.back().vardefs.emplace_back(node);
}

void Scope::add_fdef(Node *node)
{
    m_fdefs.emplace_back(node);
}

Node *Scope::find_vardef(const std::string &name)
{
    for (auto &l : m_layers)
    {
        for (auto &def : l.vardefs)
        {
            if (def->vardef_name == name)
                return def;
        }
    }

    return 0;
}

Node *Scope::find_fdef(const std::string &name)
{
    for (auto &def : m_fdefs)
    {
        if (def->fdef_name == name)
            return def;
    }

    return 0;
}

void Scope::push_layer()
{
    m_layers.emplace_back(ScopeLayer());
}

void Scope::pop_layer()
{
    m_layers.pop_back();
}


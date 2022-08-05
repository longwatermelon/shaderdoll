#include "scope.h"

Scope::Scope()
{
}

Scope::~Scope()
{
}

void Scope::add_vardef(Node *node)
{
    m_vardefs.emplace_back(node);
}

void Scope::add_fdef(Node *node)
{
    m_fdefs.emplace_back(node);
}

Node *Scope::find_vardef(const std::string &name)
{
    for (auto &def : m_vardefs)
    {
        if (def->vardef_name == name)
            return def;
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


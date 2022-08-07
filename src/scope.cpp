#include "scope.h"
#include <exception>
#include <fmt/core.h>

Scope::Scope()
{
    push_layer();
}

Scope::~Scope()
{
}

void Scope::add_vardef(Node *node)
{
    if (find_vardef(node->vardef_name))
    {
        throw std::runtime_error(fmt::format(
            "[Scope::add_vardef] Error: Variable '{}' already exists.", node->vardef_name));
    }

    m_layers.back().vardefs.emplace_back(node);
}

void Scope::add_global_vardef(Node *node)
{
    m_global_vars.emplace_back(node);
}

void Scope::add_param(std::unique_ptr<Node> node)
{
    m_layers.back().params.emplace_back(std::move(node));
}

void Scope::add_fdef(Node *node)
{
    m_fdefs.emplace_back(node);
}

Node *Scope::find_vardef(const std::string &name)
{
    for (auto &def : m_layers.back().vardefs)
    {
        if (def->vardef_name == name)
            return def;
    }

    for (auto &param : m_layers.back().params)
    {
        if (param->vardef_name == name)
            return param.get();
    }

    for (auto &def : m_global_vars)
    {
        if (def->vardef_name == name)
            return def;
    }

    return nullptr;
}

Node *Scope::find_fdef(const std::string &name)
{
    for (auto &def : m_fdefs)
    {
        if (def->fdef_name == name)
            return def;
    }

    return nullptr;
}

void Scope::push_layer()
{
    m_layers.emplace_back(ScopeLayer());
}

void Scope::pop_layer()
{
    m_layers.pop_back();
}


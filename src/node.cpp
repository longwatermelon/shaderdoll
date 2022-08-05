#include "node.h"

Node::Node(NodeType type)
    : type(type)
{
}

std::string Node::dtype2str(NodeType type)
{
    switch (type)
    {
    case NodeType::FLOAT: return "float";
    case NodeType::VOID: return "void";
    default: return "";
    }
}

NodeType Node::str2dtype(const std::string &s)
{
    if (s == "float") return NodeType::FLOAT;
    if (s == "void") return NodeType::VOID;

    return NodeType::NOOP;
}


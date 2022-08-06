#pragma once
#include <string>
#include <memory>
#include <vector>

enum class NodeType
{
    NOOP = 0,
    VARDEF,
    VAR,
    FCALL,
    FDEF,
    ASSIGN,
    PARAM,
    FLOAT,
    VOID,
    CONSTRUCTOR,
    COMPOUND
};

struct Node
{
    NodeType type;

    // vardef
    std::string vardef_name;
    std::unique_ptr<Node> vardef_value;
    NodeType vardef_type = NodeType::NOOP;

    // var
    std::string var_name;

    // fcall
    std::string fcall_name;
    std::vector<std::unique_ptr<Node>> fcall_args;

    // fdef
    std::string fdef_name;
    std::vector<std::unique_ptr<Node>> fdef_params;
    std::unique_ptr<Node> fdef_body;
    NodeType fdef_rtype = NodeType::NOOP; // return

    // assign
    std::unique_ptr<Node> assign_l, assign_r;

    // param
    NodeType param_type = NodeType::NOOP;
    std::string param_name;

    // float
    float float_value = 0.f;

    // constructor
    NodeType ctor_type = NodeType::NOOP;
    std::vector<std::unique_ptr<Node>> ctor_args;
    std::unique_ptr<Node> ctor_res;

    // compound
    std::vector<std::unique_ptr<Node>> comp_values;

    Node(NodeType type);

    std::unique_ptr<Node> copy();

    static std::string dtype2str(NodeType type);
    static NodeType str2dtype(const std::string &s);
};

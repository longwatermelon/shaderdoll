#include "parser.h"
#include "visitor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <fmt/core.h>

void variables(Visitor &v, size_t x, size_t y, size_t size)
{
    std::unique_ptr<Node> vardef = std::make_unique<Node>(NodeType::VARDEF);

    std::unique_ptr<Node> uv = std::make_unique<Node>(NodeType::VEC);
    uv->vec_values.resize(2);
    float xp = static_cast<float>(x) / static_cast<float>(size);
    float yp = static_cast<float>(y) / static_cast<float>(size);

    uv->vec_values[0] = std::make_unique<Node>(NodeType::FLOAT);
    uv->vec_values[1] = std::make_unique<Node>(NodeType::FLOAT);

    uv->vec_values[0]->float_value = xp;
    uv->vec_values[1]->float_value = yp;

    vardef->vardef_value = std::move(uv);
    vardef->vardef_type = NodeType::VEC;
    vardef->vardef_name = "uv";

    v.add_input(std::move(vardef));
}

void generate(const std::string &prog, size_t size)
{
    std::ifstream ifs(prog);
    std::stringstream ss;
    std::string buf;

    while (std::getline(ifs, buf))
        ss << buf << "\n";

    ifs.close();

    Parser parser(ss.str());
    std::unique_ptr<Node> root = parser.parse();
    root->comp_values.emplace_back(std::make_unique<Node>(NodeType::FCALL));
    root->comp_values[root->comp_values.size() - 1]->fcall_name = "main";

    std::ofstream ofs("out.ppm");
    ofs << "P3\n" << size << ' ' << size << "\n255\n";

    for (size_t y = 0; y < size; ++y)
    {
        for (size_t x = 0; x < size; ++x)
        {
            Visitor v;
            variables(v, x, y, size);

            std::unique_ptr<Node> root_copy = root->copy();
            v.visit(root_copy.get());

            auto &rgb = root_copy->comp_values[root->comp_values.size() - 1]->fcall_ret->vec_values;
            int r = std::clamp(rgb[0]->float_value * 255.f, 0.f, 255.f);
            int g = std::clamp(rgb[1]->float_value * 255.f, 0.f, 255.f);
            int b = std::clamp(rgb[2]->float_value * 255.f, 0.f, 255.f);

            ofs << r << ' ' << g << ' ' << b << "\n";
        }
    }
}

int main(int argc, char **argv)
{
    try
    {
        generate(argv[1], 100);
    }
    catch (std::runtime_error &e)
    {
        fmt::print("{}\n", e.what());
    }

    return 0;
}


#include "parser.h"
#include "visitor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

void generate()
{
    std::ifstream ifs("prog");
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
    ofs << "P3\n500 500\n255\n";

    for (size_t y = 0; y < 500; ++y)
    {
        for (size_t x = 0; x < 500; ++x)
        {
            Visitor v;

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

int main()
{
    generate();
    return 0;
}


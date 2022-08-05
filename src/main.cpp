#include "parser.h"
#include "visitor.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main()
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

    Visitor visitor;
    visitor.visit(root.get());

    std::cout << "hoo haw hee hee\n";

    return 0;
}


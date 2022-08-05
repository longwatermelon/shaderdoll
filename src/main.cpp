#include "parser.h"
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
    std::cout << "hoo haw hee hee\n";

    return 0;
}


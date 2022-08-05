#include "lexer.h"
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

    Lexer lexer(ss.str());
    Token t(TokenType::ID, "");

    while ((t = lexer.next_token()).type != TokenType::EOF_)
    {
        std::cout << (int)t.type << " | " << t.value << "\n";
    }

    return 0;
}


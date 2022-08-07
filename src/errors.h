#pragma once
#include "node.h"
#include "token.h"

namespace errors
{
    namespace lexer
    {
        void unrecognized_token(size_t line, char tok);
    }

    namespace parser
    {
        void unexpected_token(const Token &tok, TokenType expected);
    }
}


#pragma once

#include <vector>

#include "parser.hpp"
#include "scanner.h"

namespace up
{
    class Compiler
    {
    public:
        Compiler();
        
    public:
        // Returns 0 if no error
        int parse(const std::string &FILE_PATH);
        
    public:
        void printLetter(const char l)
        { std::cout << l << '\n'; }

        void printNumber(const int i)
        { std::cout << i << '\n'; }
        
    private:
        Scanner scanner;
        Parser parser;
    };
}

#pragma once

#include <vector>
#include <string>

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
        // TODO : Remove these test functions
        void printLetter(const char l)
        { std::cout << l << '\n'; }

        void printNumber(const int i)
        { std::cout << i << '\n'; }

        void printVarDecl(const std::string &TYPE, const std::string &ID, const int VAL)
        { std::cout << TYPE << " : " << ID << " := " << VAL << endl; }
        
    private:
        Scanner scanner;
        Parser parser;
    };
}

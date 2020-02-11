#pragma once

#include <vector>
#include <string>

#include "parser.hpp"
#include "scanner.h"
#include "components.h"

namespace up
{
    class Compiler
    {
    public:
        Compiler();
        
    public:
        // Returns 0 if no error
        // Calls scan and generate
        int parse(const std::string &FILE_PATH);
        
    public:
        // !!! Tests //
        // TODO : Remove these test functions
        void printLetter(const char l)
        { std::cout << l << '\n'; }

        void printNumber(const int i)
        { std::cout << i << '\n'; }

        void varDecl(const typeId &TYPE, const identifier &ID, const std::string &VAL)
        {
            Variable var(ID, TYPE);
            var.initVal = VAL;
            vars.push_back(var);
        }
        // !!! End tests //
        
    private:
        // Calls the scanner to create components
        int scan(const std::string &FILE_PATH);
        // Generates the program with all scanned components
        int generate();

    private:
        Scanner scanner;
        Parser parser;

        // !!! Tests //
        std::vector<Variable> vars;
        // !!! End tests //

        // The program in string
        std::string program;
    };
}

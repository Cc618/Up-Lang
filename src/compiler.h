#pragma once

#include <vector>
#include <string>

#include "scanner.h"
#include "components.h"
#include "parser.hpp" 

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

        void varDecl(const std::string &TYPE, const std::string &ID, const Literal &VAL)
        {
            Variable var(ID, TYPE);
            
            if (VAL.compatibleType(var.type))
                var.type = VAL.type;
            else
            {
                // TODO : Error with Parser::error with location
                cerr << "Error for variable \n";
                cerr << "Literal initialization must match the type of the variable\n";
                return;
            }

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

#pragma once

#include <vector>
#include <string>

#include "scanner.h"
#include "components.h"
#include "parser.hpp" 
#include "module.h"

namespace up
{
    // Main class which parses and then transpile the up code
    class Compiler
    {
    public:
        Compiler();
        ~Compiler();
        
    public:
        // Returns 0 if no error
        // Calls scan and generate
        int parse(const std::string &FILE_PATH);
        
        // Adds the module as import
        void import(const Module &MOD);

    public:
        // !!! Tests //
        std::vector<Statement*> statements;
        // !!! End tests //
        
    private:
        // Calls the scanner to create components
        int scan(const std::string &FILE_PATH);
        
        // Generates the program with all scanned components
        int generate();

        // Add all imports to program
        void parseModules();

    private:
        Scanner scanner;
        Parser parser;

        // All depedencies to import
        std::vector<Module> modules;

        // The program in string
        std::string program;
    };
}

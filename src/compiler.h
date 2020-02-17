#pragma once

#include <vector>
#include <string>

#include "scanner.h"
#include "components.h"
#include "parser.hpp" 
#include "module.h"
#include "error_info.h"

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

        // Creates and display a generation error
        void generateError(const std::string &MSG, const ErrorInfo &INFO);

        // Returns the main function
        inline Function *main()
        { return functions[0]; }

    public:
        // The first function is the main function
        std::vector<Function*> functions;
        
    private:
        // Calls the scanner to create components
        int scan(const std::string &FILE_PATH);
        
        // Generates the program with all scanned components
        void generate();

        // Add all imports to program
        void parseModules();

        // Removes each function in functions
        void clearFunctions();

    private:
        Scanner scanner;
        Parser parser;

        // All depedencies to import
        std::vector<Module> modules;

        // The program in string
        std::string program;

        // Whether the generation contains errors
        bool generationError = false;
    };
}

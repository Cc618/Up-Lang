#pragma once

#include <vector>
#include <string>
#include <set>
#include <queue>

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
        // Compiles the main up source file
        // Returns 0 if no error
        int parse(const std::string &FILE_PATH);

        // Creates and display a generation error
        void generateError(const std::string &MSG, const ErrorInfo &INFO);

        // Returns the main function
        inline Function *main()
        { return functions[0]; }
    
        // Finds a function
        // !!! Can return nullptr if the function is not found
        Function *getFunction(const std::string &ID, const std::vector<std::string> &ARG_TYPES);

    public: // Functions used in the parser
        // Adds the module as import
        void import(Module mod);
        // Adds a function to the functions list
        void addFunction(Function *f);

    public:
        // The first function is the main function
        std::vector<Function*> functions;
        
    private:
        // Calls the scanner to create components
        int scan(const Module &MOD);
        
        // Generates the program with all scanned components
        void generate();

        // Removes each function in functions
        void clearFunctions();

    private:
        Scanner scanner;
        Parser parser;

        // Already parsed modules (up / c)
        std::set<Module> parsedModules;
        // Up source files to parse 
        std::queue<Module> toParseModules;
        // Files to include in the C source
        std::set<string> includes;
        
        // The main file (entry)
        string mainFile;

        // The program in string
        std::string program;

        // Whether the generation contains errors
        bool generationError = false;
    };
}

#pragma once

#include <string>
#include <vector>
#include <set>
#include <queue>

#include "scanner.h"
#include "components.h"
#include "parser.hpp" 
#include "module.h"
#include "error_info.h"

namespace up
{
    class TypeDecl;

    // Main class which parses and then transpile the up code
    class Compiler
    {
    public:
        Compiler();
        ~Compiler();
        
    public:
        // Compiles the main Up source file to C
        // Writes the content to programOut
        // Returns 0 if no error
        int parse(const std::string &FILE_PATH, std::ostream &programOut);

        // Creates and display a generation error
        void generateError(const std::string &MSG, const ErrorInfo &INFO, const std::string &REASON="Generation");

        // TODO : Do this only for main module (error otherwise)
        // Appends a statement to the main function
        // If the statement is a C section, this
        // section will be global
        void pushGlobalStatement(Statement *s);

    public: // Functions used in the parser
        // Adds the module as import
        void import(Module mod, const ErrorInfo &INFO);

        // Adds a new type (with obj keyword)
        void newType(TypeDecl &type);
        
        // Adds a function to the functions list
        void addFunction(Function *f);

        // Add c section in global scope
        inline void addGlobalCCode(const std::string &CODE)
        { globalCCode += "\n" + CODE + "\n"; }

        // Finds a function
        // !!! Can return nullptr if the function is not found
        // TODO : ARG_TYPES with Id
        Function *getFunction(const Id &ID);
        Function *getFunction(const Id &ID, const std::vector<std::string> &ARG_TYPES);

        // Returns the variable in the current scope
        // !!! Might return nullptr
        Variable *getVar(const Id &ID);

    public:
        // The first function is the main function
        std::vector<Function*> functions;
        
        // Gathers all blocks in the process function
        // Used to find variables in the process function
        std::vector<Block*> scopes;

    private:
        // Returns the main function
        inline Function *main()
        { return functions[0]; }

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
        // (ErrorInfo is the data from where the module is imported)
        std::queue<std::pair<Module, ErrorInfo>> toParseModules;
        // Files to include in the C source
        std::set<string> includes;

        // The main file (entry)
        std::string mainFile;

        // The program in string
        std::string program;
        
        // C code sections (global scope)
        std::string globalCCode;

        // Whether the generation contains errors
        bool generationError = false;
    };
}

#pragma once

#include <string>
#include <fstream>

#include "parser.hpp"

#if ! defined(yyFlexLexerOnce)
# undef yyFlexLexer
# define yyFlexLexer UpFlexLexer
# include <FlexLexer.h>
#endif

// Set the lex function
#undef YY_DECL
#define YY_DECL up::Parser::symbol_type up::Scanner::Next()

namespace up
{
    class Compiler;

    class Scanner : public UpFlexLexer
    {
        friend class Parser;

    public:
        Scanner(Compiler &compiler);

        virtual ~Scanner()
        {}

    public:
        // Equivalent to yylex
        virtual Parser::symbol_type Next();

        // Reset attributes to parse a new file
        // Returns whether there is no error
        bool BeginParse(const std::string &FILE_PATH);
        void EndParse();

        // Moves the cursor
        void UpdateLocation(const int COLS);

    public:
        // The location within the file
        Parser::location_type loc;

    private:
        Compiler &compiler;

        // File path and content
        std::string file;
        std::ifstream fileInput;
    };

}

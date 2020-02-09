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
#define YY_DECL up::Parser::symbol_type up::Scanner::next()

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
        virtual Parser::symbol_type next();

        // Reset attributes to parse a new file
        // Returns whether there is no error
        bool beginParse(const std::string &FILE_PATH);
        void endParse();

        // Moves the cursor
        void updateLocation(const int COLS);

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

#pragma once

#include <string>
#include <fstream>

#include "components.h"
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
        // Updates the indentation
        // - TEXT : The "\n\t\t\t..." sequence
        // - LEN : Length of the whole text (LEN >= 1)
        void updateIndent(const char *TEXT, const int LEN);

        // Counts the number of tabs in TEXT
        // !!! TEXT must have either tabs or spaces
        int countTabs(const char *TEXT, const int LEN) const;

        // Creates a bool true or false for yes or no
        Literal genBoolLiteral(const std::string &LITERAL) const;

    private:
        Compiler &compiler;

        // File path and content
        std::string file;
        std::ifstream fileInput;

        // Current indentation
        // * Used when tokenizing
        int indent;

        // To add multiple indent or dedent tokens at once
        // indentsToAdd > 0 for INDENT, DEDENT otherwise
        // * Used when tokenizing
        int indentsToAdd;
    };

}

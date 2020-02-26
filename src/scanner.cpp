#include "scanner.h"

using namespace std;

namespace up
{
    Scanner::Scanner(Compiler &compiler)
        : compiler(compiler)
    {}

    Parser::symbol_type Scanner::nextToken()
    {
        while (true)
        {
            // Push token if necessary
            if (!tokens.empty())
            {
                auto tok = tokens.front();
                tokens.pop_front();

                if (tok.token() == Parser::token::TOKEN_END)
                {
                    if (ended)
                        return Parser::make_DOUBLE_END(loc);

                    ended = true;
                }
                // Update indentation
                else if (tok.token() == Parser::token::TOKEN_INDENT_UPDT)
                {
                    updateIndent(tok.value.as<int>());
                    continue;
                }

                // Check next token and update indent
                if (tok.token() == Parser::token::TOKEN_NL)
                {
                    auto nextTok = tokens.front();

                    // Update indentation only if the next token is not an indent update
                    if (nextTok.token() != Parser::token::TOKEN_INDENT_UPDT)
                        // Set indent to 0
                        updateIndent(0);
                }

                return tok;
            }

            // TODO : Bug when only one function in a file : infinite end token

            // Find the next token, tokens is empty
            Parser::symbol_type sym = next();
            
            // Special tokens
            switch (sym.token())
            {
            case Parser::token::TOKEN_NL:
                // Used to update indents if there is no tabs after
                tokens.push_back(sym);

                // Ignore consecutive new lines
                while (true)
                {
                    auto nextSym = next();

                    if (nextSym.token() != Parser::token::TOKEN_NL)
                    {
                        // Push next token
                        tokens.push_back(nextSym);
                        break;
                    }
                };
                break;

            case Parser::token::TOKEN_END:
            case Parser::token::TOKEN_INDENT_UPDT:
                // Parse it in tokens part
                tokens.push_back(sym);
                break;

            default:
                // Normal token, return it
                return sym;
            }
        }
    }

    bool Scanner::beginParse(const Module &MOD)
    {
        loc = Parser::location_type();
        indent = 0;
        module = MOD;
        tokens = std::deque<Parser::symbol_type>();
        tokens.push_back(Parser::make_START(loc));
        ended = false;

        fileInput.open(MOD.path());

        if (fileInput.is_open())
        {
            switch_streams(fileInput, cout);
            return true;
        }

        cerr << "Error : File " << MOD.path() << " can't be opened\n";

        return false;
    }

    void Scanner::endParse()
    {
        fileInput.close();
    }

    void Scanner::updateIndent(const int NEW_INDENT)
    {
        // How many tabs have been added since the last line
        int gap = NEW_INDENT - indent;
        indent = NEW_INDENT;

        // Push indents before next token
        if (gap > 0)
            for (int i = 0; i < gap; ++i)
                tokens.push_front(Parser::make_INDENT(loc));
        else
            for (int i = 0; i < -gap; ++i)
                tokens.push_front(Parser::make_DEDENT(loc));
    }

    int Scanner::countTabs(const char *TEXT, const int LEN) const
    {
        int tabs = 0;

        for (int i = 0; i < LEN; )
            if (TEXT[i] == '\t')
            {
                ++tabs;
                ++i;
            }
            else if (TEXT[i] == ' ')
            {
                // There are necessarily 4 spaces in a row because of the regex rule
                ++tabs;
                i += 4;
            }

        return tabs;
    }

    void Scanner::updateLocation(const int COLS)
    {
        loc.step();
        loc.columns(COLS);
    }

    ErrorInfo Scanner::errorInfo() const
    {
        return ErrorInfo(module, loc.begin.line, loc.begin.column);
    }
} // namespace up

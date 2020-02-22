#include "scanner.h"

using namespace std;

namespace up
{
    Scanner::Scanner(Compiler &compiler)
        : compiler(compiler)
    {}

    Parser::symbol_type Scanner::nextToken()
    {
        do
        {
            // Push token if necessary
            if (!tokens.empty())
            {
                auto tok = tokens.front();
                tokens.pop();

                // Check next token and update indent
                if (tok.token() == Parser::token::TOKEN_NL)
                {
                    auto nextTok = tokens.front();

                    if (nextTok.token() != Parser::token::TOKEN_PASS)
                    {
                        // Set indent to 0
                        updateIndent(nullptr, 0);
                    }
                    else
                        // Ignore pass token
                        tokens.pop();
                }

                return tok;
            }

            Parser::symbol_type sym = next();

            // Special tokens
            switch (sym.token())
            {
            case Parser::token::TOKEN_NL:
                // Used to update indents if there is no tabs after
                tokens.push(sym);
                // Push next token
                tokens.push(next());
                break;

            case Parser::token::TOKEN_PASS:
                // Ignore token if the token is the pass token
                break;

            default:
                // Normal token, return it
                return sym;
            }
        } while (true);
    }

    bool Scanner::beginParse(const std::string &FILE_PATH)
    {
        loc = Parser::location_type();
        indent = 0;
        file = FILE_PATH;
        tokens = std::queue<Parser::symbol_type>();
        tokens.push(Parser::make_START(loc));

        fileInput.open(FILE_PATH);

        if (fileInput.is_open())
        {
            switch_streams(fileInput, cout);
            return true;
        }

        cerr << "Error : File " << FILE_PATH << " can't be opened\n";

        return false;
    }

    void Scanner::endParse()
    {
        fileInput.close();
    }

    void Scanner::updateIndent(const char *TEXT, const int LEN)
    {
        // How many tabs have been added since the last line
        int gap;

        if (LEN == 0)
        {
            // There is no tabs
            gap = -indent;
            indent = 0;
        }
        else
        {
            const int TABS = countTabs(TEXT, LEN);
            gap = TABS - indent;

            // Update indentation
            indent = TABS;
        }

        // Push indents
        if (gap > 0)
            for (int i = 0; i < gap; ++i)
                tokens.push(Parser::make_INDENT(loc));
        else
            for (int i = 0; i < -gap; ++i)
                tokens.push(Parser::make_DEDENT(loc));

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
        return ErrorInfo(file, loc.begin.line, loc.begin.column);
    }
} // namespace up

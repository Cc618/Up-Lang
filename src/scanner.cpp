#include "scanner.h"

using namespace std;

namespace up
{
    Scanner::Scanner(Compiler &compiler)
        : compiler(compiler)
    {}

    bool Scanner::beginParse(const std::string &FILE_PATH)
    {
        loc = Parser::location_type();
        indent = 0;
        indentsToAdd = 0;
        file = FILE_PATH;

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
        const int TABS = countTabs(TEXT + 1, LEN - 1);

        // How many tabs have been added since the last line
        const int GAP = TABS - indent;

        indentsToAdd = GAP;

        // Update indentation
        indent = TABS;
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

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
        // TODO : With 4 spaces = 1 tab also
        const int TABS = LEN - 1;

        // How many tabs have been added since the last line
        const int GAP = TABS - indent;

        // if (GAP > 0)
        // {
        //     std::cout << GAP << " INDENTS\n";
        // }
        // else if (GAP < 0)
        //     std::cout << (-GAP) << " DEDENTS\n";

        indentsToAdd = GAP;

        // Update indentation
        indent = TABS;
    }

    void Scanner::updateLocation(const int COLS)
    {
        loc.step();
        loc.columns(COLS);
    }
} // namespace up

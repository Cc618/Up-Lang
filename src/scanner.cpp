#include "scanner.h"

using namespace std;

namespace up
{
    Scanner::Scanner(Compiler &compiler)
        : compiler(compiler)
    {}

    bool Scanner::BeginParse(const std::string &FILE_PATH)
    {
        file = FILE_PATH;
        loc = Parser::location_type();

        fileInput = ifstream(FILE_PATH);

        if (fileInput.is_open())
        {
            switch_streams(fileInput, cout);
            return true;
        }
        
        return false;
    }

    void Scanner::EndParse()
    {
        fileInput.close();
    }

    void Scanner::UpdateLocation(const int COLS)
    {
        loc.step();
        loc.columns(COLS);
    }
} // namespace up

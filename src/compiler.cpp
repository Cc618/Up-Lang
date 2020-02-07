#include "compiler.h"

#include <sstream>

namespace up
{
    Compiler::Compiler()
        : scanner(*this), parser(scanner, *this)
    {}

    int Compiler::Parse(const std::string &FILE_PATH)
    {
        if (!scanner.BeginParse(FILE_PATH))
            return -1;

        int ret = parser.parse();

        scanner.EndParse();

        return ret;
    }
} // namespace up

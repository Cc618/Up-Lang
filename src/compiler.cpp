#include "compiler.h"

#include <sstream>

namespace up
{
    Compiler::Compiler()
        : scanner(*this), parser(scanner, *this)
    {}

    int Compiler::parse(const std::string &FILE_PATH)
    {
        if (!scanner.beginParse(FILE_PATH))
        {
            scanner.endParse();
            return -1;
        }

        int ret = parser.parse();

        scanner.endParse();

        return ret;
    }
} // namespace up

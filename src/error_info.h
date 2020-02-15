#pragma once

// Used to handle error printing

#include <string>

namespace up
{
    struct ErrorInfo
    {
        ErrorInfo() = default;
        ErrorInfo(const std::string &FILE, const unsigned int LINE, const unsigned int COL)
            : file(FILE), line(LINE), column(COL)
        {}

        std::string file;
        unsigned int line;
        unsigned int column;
    };
} // namespace up

#pragma once

// Used to handle error printing

#include "module.h"

namespace up
{
    struct ErrorInfo
    {
        ErrorInfo() = default;
        ErrorInfo(const Module &FILE, const unsigned int LINE, const unsigned int COL)
            : file(FILE), line(LINE), column(COL)
        {}

        Module file;
        unsigned int line;
        unsigned int column;
    };
} // namespace up

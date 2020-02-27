#pragma once

// Used to handle error printing

#include "module.h"

namespace up
{
    struct ErrorInfo
    {
        // Empty error data
        static inline ErrorInfo empty()
        {
            Module mod;
            mod.folder = "<unknown>";
            return ErrorInfo(mod, 0, 0);
        }

        ErrorInfo() = default;
        ErrorInfo(const Module &FILE, const unsigned int LINE, const unsigned int COL)
            : file(FILE), line(LINE), column(COL)
        {}

        Module file;
        unsigned int line;
        unsigned int column;
    };
} // namespace up

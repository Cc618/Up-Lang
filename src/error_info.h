#pragma once

// Used to handle error printing

#include <string>
#include <sstream>

#include "module.h"
#include "colors.h"

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

        inline std::string toString() const
        {
            std::stringstream s;
            
            s << "File " << AS_GREEN_S(file.path()) <<
                ":" << AS_YELLOW_S(line) << ":" << AS_YELLOW_S(column);

            return s.str();
        }

        Module file;
        unsigned int line;
        unsigned int column;
    };
} // namespace up

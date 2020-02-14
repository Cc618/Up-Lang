#pragma once

// A module is a file to import (c header or up source)

#include <string>

namespace up
{
    class Module
    {
    public:
        Module() = default;
        Module(const std::string &NAME, const bool UP=true)
            : name(NAME), up(UP)
        {}

        // Can be a path : mod.file
        std::string name;
        // Whether the module is a c header
        // or an up file 
        bool up;
    };
}

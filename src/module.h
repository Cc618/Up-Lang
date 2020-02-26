#pragma once

// A module is a file to import (c header or up source)

#include <string>

namespace up
{
    class Module
    {
    public:
        Module() = default;
        explicit Module(const std::string &NAME, const bool UP=true, const std::string &FOLDER=".")
            : name(NAME), up(UP), folder(FOLDER)
        {}

    public:
        // Returns the absolute path
        inline std::string path() const
        {
            // TODO : Absolute
            return folder + "/" + name;
        }

    public:
        // To use it in a set
        inline bool operator<(const Module &MOD) const
        { return name[0] < MOD.name[0]; }

    public:
        // Can be a path : mod.file
        std::string name;
        std::string folder;
        // Whether the module is a c header
        // or an up file 
        bool up;
    };
}

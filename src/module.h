#pragma once

// A module is a file to import (c header or up source)

#include <string>

#include "id.h"

namespace up
{
    class Module
    {
    public:
        Module() = default;
        explicit Module(const Id &ID, const bool UP=true, const std::string &FOLDER=".")
            : id(ID), up(UP), folder(FOLDER)
        {}

    public:
        // Returns the absolute path
        inline std::string path() const
        {
            // TODO : Absolute
            return folder + "/" + id.toPath();
        }

    public:
        // To use it in a set
        inline bool operator<(const Module &MOD) const
        { return id.ids[0][0] < MOD.id.ids[0][0]; }

    public:
        // Can be a path : mod.file
        Id id;
        // The folder where the module has been imported (relative folder)
        std::string folder;
        // Whether the module is a c header
        // or an up file 
        bool up;
    };
}

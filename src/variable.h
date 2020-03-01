#pragma once

// Variable class (semantics)

#include <string>

namespace up
{
    class Variable
    {
    public:
        // !!! The type is the Up type
        Variable(const std::string &ID, const std::string &TYPE)
            : id(ID), type(TYPE)
        {}

    public:
        std::string id;
        std::string type;
    }; 
} // namespace up

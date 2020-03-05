#pragma once

// Variable class (semantics)

#include <string>

namespace up
{
    class Variable
    {
    public:
        // !!! The type is the Up type
        Variable(const Id &ID, const Id &TYPE)
            : id(ID), type(TYPE)
        {}

    public:
        Id id;
        Id type;
    }; 
} // namespace up

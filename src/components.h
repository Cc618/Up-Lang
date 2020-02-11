#pragma once

// All classes parsed and then compiled

#include <string>

namespace up
{
    // "int", "bool"...
    typedef std::string typeId;

    // Name of variable, function...
    typedef std::string identifier;

    struct Variable
    {
        Variable(const identifier &ID, const typeId &TYPE);

        // TODO : rm
        // !!! Tests //
        std::string initVal;
        std::string declare() const
        { return type + " " + id + " = " + initVal + ";"; }
        // !!! End tests //

        identifier id; 
        typeId type;
    }; 

}

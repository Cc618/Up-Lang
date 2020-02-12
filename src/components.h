#pragma once

// All classes parsed and then compiled

#include <string>

namespace up
{
    // Casts up type to c type in string
    std::string cType(const std::string &TYPE);


    // A literal expression
    // Matches a type
    struct Literal
    {
        Literal() = default;
        Literal(const std::string &DATA, const std::string &TYPE)
            : data(DATA), type(TYPE)
        {}

        // Whether the type is compatible to this type
        // * TYPE belongs a Variable
        bool compatibleType(const std::string &TYPE) const
        {
            return TYPE == "auto"
                || TYPE == type;
        }

        std::string data;
        std::string type;
    };

    struct Variable
    {
        Variable() = default;
        Variable(const std::string &ID, const std::string &TYPE);

        // TODO : rm
        // !!! Tests //
        Literal initVal;
        std::string declare() const
        { return cType(type) + " " + id + " = " + initVal.data + ";"; }
        // !!! End tests //

        std::string id; 
        std::string type;
    }; 


}

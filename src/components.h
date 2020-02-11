#pragma once

// All classes parsed and then compiled

#include <string>

namespace up
{
    // Name of variable, function...
    typedef std::string Identifier;

    // Describes a type identifier
    // Provides c type and up type
    struct TypeId
    {
        TypeId() = default;
        TypeId(const Identifier &ID);

        // Returns the c version of this type
        // !!! The type cannot be auto
        Identifier cType() const;
    
        // Up type
        Identifier id;
    };

    // A literal expression
    // Matches a type
    struct Literal
    {
        Literal() = default;
        Literal(const std::string &DATA, const TypeId &TYPE)
            : data(DATA), type(TYPE)
        {}

        // Whether the type is compatible to this type
        // * TYPE belongs a Variable
        bool compatibleType(const TypeId &TYPE) const
        {
            return TYPE.id == "auto"
                || TYPE.id == type.id;
        }

        std::string data;
        TypeId type;
    };

    struct Variable
    {
        Variable() = default;
        Variable(const Identifier &ID, const TypeId &TYPE);

        // TODO : rm
        // !!! Tests //
        Literal initVal;
        std::string declare() const
        { return type.cType() + " " + id + " = " + initVal.data + ";"; }
        // !!! End tests //

        Identifier id; 
        TypeId type;
    }; 


}

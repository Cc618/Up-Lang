#pragma once

// All classes parsed and then compiled

#include <string>

namespace up
{
    // Casts up type to c type in string
    std::string cType(const std::string &TYPE);

    // Gives a result
    // For example :
    // 42 + 618
    struct Expression
    {
        Expression() = default;

        // Returns the string representation of
        // the expression in C
        virtual std::string toString() const = 0;
    };

    // A literal expression
    // Matches a type
    // For example :
    // 42
    class Literal : public Expression
    {
    public:
        Literal() = default;
        Literal(const std::string &DATA, const std::string &TYPE)
            : data(DATA), type(TYPE)
        {}

    public:
        // Whether the type is compatible to this type
        // * TYPE belongs to a Variable
        // !!! Use up types
        bool compatibleType(const std::string &TYPE) const;

        virtual std::string toString() const override;

    public:
        // Up type
        std::string type;
    
    private:
        std::string data;
    };

    struct Variable
    {
        Variable() = default;
        Variable(const std::string &ID, const std::string &TYPE);

        // TODO : rm
        // !!! Tests //
        Literal initVal;
        std::string declare() const
        { return cType(type) + " " + id + " = " + initVal.toString() + ";"; }
        // !!! End tests //

        std::string id; 
        std::string type;
    }; 


}

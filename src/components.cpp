#include "components.h"

#include <iostream>

using namespace std;

namespace up
{
    std::string cType(const std::string &id)
    {
        // TODO : Error
        if (id == "auto")
            return "auto_type";
        
        if (id == "num")
            return "float";
        
        return id;
    }

    Expression::Expression(const std::string &TYPE)
        : type(TYPE)
    {}

    bool Expression::compatibleType(const std::string &TYPE) const
    {
        return TYPE == "auto"
            || TYPE == type;
    }

    std::string Literal::toString() const
    {
        if (type == "bool")
            return data == "yes" ? "true" : "false";

        if (type == "num")
            return data + "f";

        return data;
    }

    VariableDeclaration::VariableDeclaration(const std::string &ID, const std::string &TYPE, const Expression *EXPR)
        : id(ID), type(TYPE), EXPR(EXPR)
    {
        // Verify type compatibility
        if (EXPR->compatibleType(type))
            type = EXPR->type;
        else
        {
            // TODO : Error with location
            std::cerr << "Error for variable '" << id << "'\n";
            std::cerr << "Literal initialization must match the type of the variable\n";
            return;
        }
    }

    VariableDeclaration::~VariableDeclaration()
    {
        delete EXPR;
    }

    std::string VariableDeclaration::toString() const
    {
        return cType(type) + " " + id + " = " + EXPR->toString() + ";";
    }

} // namespace up

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

    ExpressionStatement::ExpressionStatement(const Expression *EXPR)
        : EXPR(EXPR)
    {}

    ExpressionStatement::~ExpressionStatement()
    {
        delete EXPR;
    }

    std::string ExpressionStatement::toString() const
    {
        // Just terminate the instruction
        return EXPR->toString() + ";";
    }

    std::string Literal::toString() const
    {
        if (type == "bool")
            return data == "yes" ? "true" : "false";

        if (type == "num")
            return data + "f";

        if (type == "str")
            // Replace the single quotes by double quotes
            return '"' + data.substr(1, data.size() - 2) + '"';

        return data;
    }

    Call::~Call()
    {
        for (auto a : args)
            delete a;
    }

    std::string Call::toString() const
    {
        string s = id;

        s += "(";

        if (!args.empty())
        {
            s += args[0]->toString();

            for (size_t i = 1; i < args.size(); ++i)
                s += ", " + args[i]->toString();
        }
        
        s += ")";

        return s;
    }

    VariableDeclaration::VariableDeclaration(const std::string &ID, const std::string &TYPE, const Expression *EXPR)
        : id(ID), type(TYPE), EXPR(EXPR)
    {
        // TODO : Check variable exists

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

    VariableAssignement::VariableAssignement(const std::string &ID, const Expression *EXPR, const std::string &OP)
        : id(ID), EXPR(EXPR), operand(OP)
    {
        // TODO : Check variable exists + EXPR and variable types are compatible
    }

    VariableAssignement::~VariableAssignement()
    {
        delete EXPR;
    }

    std::string VariableAssignement::toString() const
    {
        return id + " " + operand + " " + EXPR->toString() + ";";
    }

    UnaryOperation::UnaryOperation(const std::string &ID, const std::string &OP, const bool PREFIX)
        : Expression("auto"), id(ID), operand(OP), prefix(PREFIX)
    {
        // TODO : Check variable exists + deduce type compatibility
    }

    std::string UnaryOperation::toString() const
    {
        if (prefix)
            return operand + id;
        else
            return id + operand;
    }
} // namespace up

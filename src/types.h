#pragma once

// Types related functions

#include <string>
#include <vector>

namespace up
{
    class Expression;

    // Casts up type to c type in string
    // !!! May return empty string if the type
    // !!! is an auto type
    std::string cType(const std::string &TYPE);

    // Returns the argument types list
    // All arguments have a c type
    std::vector<std::string> typeArgList(const std::vector<Expression*> &ARGS);

    // Whether both types are compatible
    bool compatibleType(const std::string &a, const std::string &b);    

    // Adds an operator for a type
    void declareOperator(const std::string &TYPE, const std::string &OP);

    // Whether this type provides this operator (OP)
    bool operatorExists(const std::string &TYPE, const std::string &OP);

    // A builtin type has its own operators in C (no function call)
    bool isBuiltin(const std::string &TYPE);
}

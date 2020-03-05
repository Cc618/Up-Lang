#pragma once

// Types related functions

#include <string>
#include <vector>

#include "id.h"
#include "error_info.h"

namespace up
{
    class Expression;
    class Compiler;

    // For example :
    // obj MyObj
    // ID : MyObj
    class TypeDecl
    {
    public:
        TypeDecl() = default;
        TypeDecl(const ErrorInfo &INFO, const Id &ID);
    
    public:
        void process(Compiler *compiler);

    public:
        ErrorInfo info;
        Id id;
    };

    // Casts up type to c type in string
    // !!! May return empty string if the type
    // !!! is an auto type
    std::string cType(const std::string &TYPE);

    // TODO : C type ?
    // Returns the argument types list
    // All arguments have a c type
    std::vector<std::string> typeArgList(const std::vector<Expression*> &ARGS);

    // Declares a new type
    void newType(const Id &ID, Compiler *compiler);

    // Whether a type already exists
    bool typeExists(const Id &ID);

    // Whether both types are compatible
    bool compatibleType(const Id &a, const Id &b);    

    // Adds an operator for a type
    void declareOperator(const Id &TYPE, const std::string &OP);

    // Whether this type provides this operator (OP)
    bool operatorExists(const Id &TYPE, const std::string &OP);

    // A builtin type has its own operators in C (no function call)
    bool isBuiltin(const Id &TYPE);
}

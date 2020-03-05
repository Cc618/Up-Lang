#include "types.h"

#include <iostream>
#include <set>

#include "compiler.h"
#include "components.h"
#include "colors.h"

using namespace std;

namespace up
{
    // TODO : += statements check types

    // Whether a type declares an operator
    // * Mangling in operatorExists
    // TODO : Change init when mangling
    // TODO : Not operator
    // TODO : _new _del etc...
    set<string> typeOperators = {
        "int+",
        "int-",
        "int*",
        "int/",
        "int%",
        "int++",
        "int--",
        "int=",
        "int<",
        "int>",
        "int<=",
        "int>=",
        "int==",
        "int!=",

        "num+",
        "num-",
        "num*",
        "num/",
        "num%",
        "int--",
        "int++",
        "num=",
        "num<",
        "num>",
        "num<=",
        "num>=",
        "num==",
        "num!=",

        "bool=",
        "bool==",
        "bool!=",
    };

    set<Id> types = {
        Id("int"),
        Id("num"),
        Id("nil"),
        Id("bool"),

        /* // TODO : Remove */ 
        Id("str"),
    };

    TypeDecl::TypeDecl(const ErrorInfo &INFO, const Id &ID)
        : info(INFO), id(ID)
    {}

    void TypeDecl::process(Compiler *compiler)
    {
        // Check simple type
        if (!id.isSimple())
        {
            compiler->generateError("The type '" + AS_BLUE(id.toUp()) +
                "' must have a simple id (no prefix)", info);
            return;
        }

        if (typeExists(id))
        {
            compiler->generateError("The type '" + AS_BLUE(id.toUp()) +
                "' already exists", info);
            return;
        }

        types.insert(id);
    }

    string cType(const string &id)
    {
        if (id == "auto")
            // Empty = error
            return "";

        if (id == "num")
            return "float";

        // TODO : Custom up string type
        if (id == "str")
            return "const char*";

        if (id == "nil")
            return "void";

        if (id == "bool")
            return "unsigned char";

        return id;
    }

    std::vector<string> typeArgList(const std::vector<Expression*> &ARGS)
    {
        vector<string> args;

        for (auto arg : ARGS)
            // TODO : Better mangling
            args.push_back(arg->type.toC());

        return args;
    }

    // TODO : rm
    void newType(const Id &ID, Compiler *compiler)
    {
        // Check already exists
        if (typeExists(ID))
        {
            // TODO : Error
            // compiler->generateError()
        }

        types.insert(ID);
    }

    bool typeExists(const Id &ID)
    {
        return types.find(ID) != types.end();
    }

    bool compatibleType(const Id &a, const Id &b)
    {
        // TODO : Implicit casts (require cast...)
        return a == b;
    }

    void declareOperator(const Id &TYPE, const string &OP)
    {
        // TODO : When templates / function overloading, add args as param and change MANGLED (also in operatorExists)
        // TODO : Up when mangled ?
        const string MANGLED = TYPE.toUp() + OP;

        typeOperators.insert(MANGLED);
    }

    bool operatorExists(const Id &TYPE, const string &OP)
    {
        // TODO : When templates / function overloading, add args as param and change MANGLED (also in declareOperator)
        // TODO : Up when mangled ?
        const string MANGLED = TYPE.toUp() + OP;

        return typeOperators.find(MANGLED) != typeOperators.end();
    }

    bool isBuiltin(const Id &TYPE)
    {
        return TYPE == "int" || TYPE == "num" || TYPE == "bool" || TYPE == "nil";
    }
} // namespace up

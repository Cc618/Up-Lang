#include "types.h"

#include <iostream>
#include <set>

#include "compiler.h"
#include "components.h"

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
            return "uint8";

        return id;
    }

    std::vector<string> typeArgList(const std::vector<Expression*> &ARGS)
    {
        vector<string> args;

        for (auto arg : ARGS)
            args.push_back(arg->type);

        return args;
    }

    bool compatibleType(const string &a, const string &b)
    {
        // TODO : Implicit casts (require cast...)
        return a == b;
    }

    void declareOperator(const string &TYPE, const string &OP)
    {
        // TODO : When templates / function overloading, add args as param and change MANGLED (also in operatorExists)
        const string MANGLED = TYPE + OP;

        typeOperators.insert(MANGLED);
    }

    bool operatorExists(const string &TYPE, const string &OP)
    {
        // TODO : When templates / function overloading, add args as param and change MANGLED (also in declareOperator)
        const string MANGLED = TYPE + OP;

        return typeOperators.find(MANGLED) != typeOperators.end();
    }

    bool isBuiltin(const string &TYPE)
    {
        return TYPE == "int" || TYPE == "num" || TYPE == "bool" || TYPE == "nil";
    }
} // namespace up

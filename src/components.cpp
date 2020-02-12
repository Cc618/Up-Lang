#include "components.h"

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

    bool Literal::compatibleType(const std::string &TYPE) const
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


    Variable::Variable(const std::string &ID, const std::string &TYPE)
        : id(ID), type(TYPE)
    {}
} // namespace up

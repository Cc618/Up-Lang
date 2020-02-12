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

    Variable::Variable(const std::string &ID, const std::string &TYPE)
        : id(ID), type(TYPE)
    {}
} // namespace up

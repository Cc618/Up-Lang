#include "components.h"

using namespace std;

namespace up
{
    TypeId::TypeId(const Identifier &ID)
        : id(ID)
    {}

    Identifier TypeId::cType() const
    {
        // TODO : Error
        if (id == "auto")
            return "auto_type";
        
        if (id == "num")
            return "float";
        
        return id;
    }

    Variable::Variable(const Identifier &ID, const TypeId &TYPE)
        : id(ID), type(TYPE)
    {}
} // namespace up

#include "components.h"

using namespace std;

namespace up
{
    Variable::Variable(const identifier &ID, const typeId &TYPE)
        : id(ID), type(TYPE)
    {}
} // namespace up

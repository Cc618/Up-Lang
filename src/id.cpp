#include "id.h"

using namespace std;

namespace up
{
    Id Id::createAuto()
    { return Id("auto"); }

    Id Id::createEllipsis()
    { return Id("..."); }

    bool Id::operator==(const Id &OTHER) const
    {
        if (ids.size() != OTHER.ids.size())
            return false;
        
        for (int i = 0; i < ids.size(); ++i)
            if (ids[i] != OTHER.ids[i])
                return false;
        
        return true;
    }

    string Id::toUp() const
    {
        string s;

        for (int i = 0; i < ids.size() - 1; ++i)
            s += ids[i] + '.';
        
        // Name after
        s += name();

        return s;
    }

    string Id::toC() const
    {
        // Name before
        string s = name();

        for (int i = 0; i < ids.size() - 1; ++i)
            s += '_' + ids[i];
        
        return s;
    }

} // namespace up

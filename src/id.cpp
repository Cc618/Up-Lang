#include "id.h"

#include <cstring>

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
    
    bool Id::operator<(const Id &OTHER) const
    {
        return strcmp(ids[0].c_str(), OTHER.ids[0].c_str()) < 0;
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
        string s;

        for (int i = 0; i < ids.size() - 1; ++i)
            s += ids[i] + '_';
        
        // Name after
        s += name();

        return s;
    }

    string Id::toPath() const
    {
        string s;

        for (int i = 0; i < ids.size() - 1; ++i)
            s += ids[i] + '/';
        
        s += name();

        return s;
    }
} // namespace up

#pragma once

// An identifier

#include <string>
#include <vector>

namespace up
{
    // Composed of multiple ids
    // For example :
    // var.fun
    // ids : { var, fun }
    class Id
    {
    public:
        Id() = default;
        Id(const std::string &s)
            : ids({ s })
        {}
        Id(const std::vector<std::string> &ids)
            : ids(ids)
        {}

    public:
        // Whether there is only one id in ids
        inline bool isSimple() const
        { return ids.size() == 1; }

    public:
        std::vector<std::string> ids;
    };
}

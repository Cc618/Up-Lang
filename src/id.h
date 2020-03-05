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
        // For auto type
        static Id createAuto();
        // ... id
        static Id createEllipsis();

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

        // Last id
        inline std::string name() const
        { return ids.back(); }

        // Return the up string representation (dots)
        // !!! Must have at least one id
        std::string toUp() const;

        // Return the c string representation (underscores)
        // TODO : Better mangling
        std::string toC() const;

    public:
        bool operator==(const Id &OTHER) const;
        // Checks only with name
        inline bool operator==(const std::string &s) const
        { return name() == s; }
        inline bool operator!=(const std::string &s) const
        { return name() != s; }

    public:
        std::vector<std::string> ids;
    };
}

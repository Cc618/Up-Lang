#include "global.h"

#include <iostream>
#include <unistd.h>
#include <libgen.h>
#include <linux/limits.h>

using namespace std;

namespace up
{
    string includeDir;

    int initGlobal()
    {
        char path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);

        if (count == -1)
        {
            cerr << "Error when reading up path\n";
            return -1;
        }

        // up is in the bin directory so go to .. and then include
        includeDir = string(dirname(dirname(path))) + "/include/";

        return 0;
    }
} // namespace up

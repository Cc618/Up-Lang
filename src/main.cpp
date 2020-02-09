#include <iostream>

#include "scanner.h"
#include "compiler.h"
#include "parser.hpp"

#include <fstream>

int main(int argc, char **argv)
{
    using namespace up;
    using namespace std;

    if (argc < 1)
    {
        cerr << "Usage : up <file>\n";

        return -1;
    }

    Compiler compiler;
    int ret = compiler.parse(argv[1]);

    if (ret)
        cerr << "Can't compile\n";

    return ret;
}

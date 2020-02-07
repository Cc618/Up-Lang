#include <iostream>

#include "scanner.h"
#include "compiler.h"
#include "parser.hpp"

int main(int argc, char **argv)
{
    using namespace up;
    using namespace std;

    Compiler compiler;
    int ret = compiler.Parse("main.up");

    if (ret)
        cerr << "Can't compile\n";

    return ret;
}

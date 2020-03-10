#include <iostream>
#include <fstream>
#include <cstdio>

#include "scanner.h"
#include "compiler.h"
#include "parser.hpp"

using namespace up;
using namespace std;

// Compiles the Up file located at 'entry' to
// the c file 'out'
void compileToCFile(const string ENTRY, const string OUT, Compiler &compiler, int &ret)
{
    ofstream outFile(OUT);

    if (outFile.is_open())
    {
        ret = compiler.parse(ENTRY, outFile);
        outFile.close();
    }
    else
    {
        cerr << "Can't open output file '" << OUT << "'\n";
        ret = -1;
    }
}

// Compiles the Up file located at 'entry' to
// the binary file 'out'
void compileToBinFile(const string ENTRY, const string OUT, Compiler &compiler, int &ret)
{
    // Create tmp file for c
    const string C_FILE = ENTRY + ".tmp.c";

    // Verify this file doesn't exist
    if (ifstream(C_FILE).good())
    {
        cerr << "Can't create a temporary file '" << C_FILE <<
            "' because this file already exists\n";
        ret = -1;
        return;
    }
    
    // Up to C
    compileToCFile(ENTRY, C_FILE, compiler, ret);
    
    // C to Bin
    string cmd = "gcc -o ";
    cmd += OUT;
    cmd += " " + C_FILE;
    system(cmd.c_str());

    // Remove it
    remove(C_FILE.c_str());
}

int main(int argc, char **argv)
{
    Compiler compiler;
    int ret;

    // Output C to stdout
    if (argc == 2)
        ret = compiler.parse(argv[1], std::cout);
    // Write file
    else if (argc == 3)
    {
        string entry = argv[1];
        string out = argv[2];

        // The output is a C file
        if (out.size() >= 2 && out.substr(out.size() - 2) == ".c")
            compileToCFile(entry, out, compiler, ret);
        // The output is a binary
        else
            compileToBinFile(entry, out, compiler, ret);
    }
    else
    {
        cerr << "Usage :\n";
        cerr << "up <entry.up>\t\tPrints the C output to stdout\n";
        cerr << "up <entry.up> <out.c>\tWrites the C output to out.c\n";
        cerr << "up <entry.up> <out>\tCompiles to the binary out (using gcc)\n";

        return -1;
    }

    if (ret)
        cerr << "Can't compile\n";

    return ret;
}

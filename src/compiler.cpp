#include "compiler.h"

#include <sstream>
#include <algorithm>

#include "global.h"
#include "colors.h"
#include "types.h"

using namespace std;

namespace up
{
    Compiler::Compiler()
        : scanner(*this), parser(scanner, *this)
    {}

    Compiler::~Compiler()
    {
        clearFunctions();
    }

    int Compiler::parse(const string &FILE_PATH, ostream &programOut)
    {
        // Invalid file name
        if (FILE_PATH.size() < 4 || FILE_PATH.substr(FILE_PATH.size() - 3) != ".up")
        {
            cerr << "The name of the file '" << FILE_PATH << "' is invalid\n";

            return -1;
        }

        // Init variables
        program = "";
        generationError = false;
        parsedModules.clear();
        toParseModules = queue<pair<Module, ErrorInfo>>();
        includes.clear();
        scopes.clear();
        mainFile = FILE_PATH;
        globalCCode = "";

        clearFunctions();
        functions.push_back(UpFunction::createMain());

        // Init scan with the first file
        int lastSlash = mainFile.find_last_of('/');

        // This path contains only the name of the file
        Module mainModule = lastSlash == string::npos ?
            Module(mainFile.substr(0, mainFile.size() - 3), true) :
            Module(mainFile.substr(lastSlash + 1, mainFile.size() - lastSlash - 4), true, mainFile.substr(0, lastSlash));

        import(mainModule, ErrorInfo::empty());

        // Scan all modules
        int ret = 0;
        // Current module being parsed
        Module mod;
        ErrorInfo modImportInfo;
        while (ret == 0 && !toParseModules.empty())
        {
            auto [first, last] = toParseModules.front();
            mod = first;
            modImportInfo = last;
            toParseModules.pop();

            ret = scan(mod);
        }

        if (ret != 0)
        {
            // Import errors
            // TODO : ErrorInfo::isEmpty
            if (mod.path() != mainFile)
                generateError("File '" + AS_BLUE(mod.path()) + "' can't be imported\n", modImportInfo);

            return ret;
        }

        // Generate
        generate();

        if (generationError)
            return 1;

        programOut << program;

        return ret;
    }

    void Compiler::generateError(const string &MSG, const ErrorInfo &INFO, const string &REASON)
    {
        generationError = true;

        cerr << "File " << INFO.toString() <<
            " - " << AS_RED_S(REASON + " Error") <<
            " :\n" << MSG << '\n';
    }

    void Compiler::pushGlobalStatement(Statement *s)
    {
        // This is a C section
        if (auto cSection = dynamic_cast<CStatement*>(s))
            addGlobalCCode(cSection->toString());
        else
            ((UpFunction*) main())->body->pushStatement(s);
    }

    void Compiler::import(Module mod, const ErrorInfo &INFO)
    {
        // Module already imported
        if (parsedModules.find(mod) != parsedModules.end())
            return;

        // TODO : Use absolute file path

        // Set this module to parsed
        parsedModules.insert(mod);

        // Special module that gathers multiple modules
        if (mod.id == "libc")
        {
            includes.insert("stdio.h");
            includes.insert("stdlib.h");
            includes.insert("math.h");
        }
        // Up module
        else if (mod.up)
        {
            mod.id.name() += ".up";
            toParseModules.push({ mod, INFO });
        }
        // C module
        else
            // Add the extension
            includes.insert(mod.path() + ".h");
    }
    
    void Compiler::newType(TypeDecl &type)
    {
        type.process(this);
    }
    
    void Compiler::addFunction(Function *f)
    {
        // Find same function
        auto i = std::find_if(functions.cbegin(), functions.cend(), [f](const Function* a) -> bool { return *a == *f; });

        // f is already in functions
        if (i != functions.cend())
        {
            generateError("The function '" + AS_BLUE(f->id.toUp()) +
                "' already exists (declared at " + (*i)->info.toString() + ")", f->info);
            return;
        }

        functions.push_back(f);
    }

    Function *Compiler::getFunction(const Id &ID)
    {
        for (auto f : functions)
            if (f->id == ID)
                return f;

        // Not found
        return nullptr;
    }

    Function *Compiler::getFunction(const Id &ID, const std::vector<std::string> &ARG_TYPES)
    {
        bool isEllipsis = ARG_TYPES.size() == 1 && ARG_TYPES[0] == "...";

        for (auto f : functions)
            if (f->id == ID)
            {
                // Ellipsis case
                if (isEllipsis || (f->args.size() == 1 && f->args[0]->isEllipsis()))
                    return f;

                // Check types
                if (f->args.size() == ARG_TYPES.size())
                {
                    bool compatible = true;
                    for (size_t i = 0; i < ARG_TYPES.size(); ++i)
                        if (!compatibleType(f->args[i]->type, ARG_TYPES[i]))
                        {
                            compatible = false;
                            break;
                        }

                    if (compatible)
                        return f;
                }
            }

        // Not found
        return nullptr;
    }

    Variable *Compiler::getVar(const Id &ID)
    {
        for (auto i = scopes.rbegin(); i != scopes.rend(); ++i)
            if (auto v = (*i)->getVar(ID))
                return v;
        
        return nullptr;
    }

    int Compiler::scan(const Module &MOD)
    {
        if (!scanner.beginParse(MOD))
        {
            scanner.endParse();
            return -1;
        }

        int ret = parser.parse();

        scanner.endParse();

        if (ret == 0 && generationError)
            return 1;

        return ret;
    }

    void Compiler::generate()
    {
        // Header //
        program += "// Code generated by the Up compiler\n\n";

        // Includes //
        // Add all includes at the head of the source file
        for (auto inc : includes)
            program += "#include \"" + inc + "\"\n";

        program += '\n';

        // Globals C Sections //
        program += globalCCode;
        program += '\n';

        // Functions //
        // TODO : Create depedencies on functions which use other functions (add signature)

        // Process functions (cdef and then up)
        // TODO : Separate CDef and UpFunction
        for (auto f : functions)
            if (f->isCDef)
                f->process(this);
        for (auto f : functions)
            if (!f->isCDef)
                f->process(this);

        // Don't stringify program if there are errors
        if (generationError)
            return;

        // Generate all functions
        for (size_t i = 1; i < functions.size(); ++i)
            if (!functions[i]->isCDef)
                program += functions[i]->toString() + "\n";

        // Add a return statement to main
        auto err = ErrorInfo::empty();
        ((UpFunction*) main())->body->
            pushStatement(new Return(err, new Literal(err, "0", Id("int"))));

        // Add the main function at the end
        program += main()->toString();
    }

    void Compiler::clearFunctions()
    {
        for (auto f : functions)
            delete f;

        functions.clear();
    }
} // namespace up

#include "compiler.h"

#include <sstream>

#include "colors.h"

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

    int Compiler::parse(const std::string &FILE_PATH)
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
                generateError("File " + mod.path() + " can't be imported\n", modImportInfo);

            return ret;
        }

        // Generate
        generate();

        if (generationError)
            return 1;

        // TODO : Write into a file
        cout << program;

        return ret;
    }

    void Compiler::generateError(const string &MSG, const ErrorInfo &INFO, const bool IS_LEX_ERROR)
    {
        generationError = true;

        cerr << "File " << YELLOW << INFO.file.path() << DEFAULT <<
            ":" << BLUE << INFO.line << DEFAULT << ":" << BLUE << INFO.column << DEFAULT <<
            " - " << RED << (IS_LEX_ERROR ? "Syntax Error" : "Generation Error") <<
            DEFAULT << " :\n" << MSG << '\n';
    }

    void Compiler::pushGlobalStatement(Statement *s)
    {
        // This is a C section
        if (auto cSection = dynamic_cast<CStatement*>(s))
            addGlobalCCode(cSection->toString());
        else
            ((UpFunction*) main())->body->content.push_back(s);
    }

    Function *Compiler::getFunction(const std::string &ID, const std::vector<std::string> &ARG_TYPES)
    {
        bool isEllipsis = ARG_TYPES.size() == 1 && ARG_TYPES[0] == "...";

        for (auto f : functions)
            // TODO : Verify arg types (with implicit casts)
            if (f->id == ID)
                if (isEllipsis || (f->args.size() == 1 && f->args[0]->isEllipsis()) ||
                    f->args.size() == ARG_TYPES.size())
                    return f;

        // Not found
        return nullptr;
    }

    void Compiler::import(Module mod, const ErrorInfo &INFO)
    {
        // Module already imported
        if (parsedModules.find(mod) != parsedModules.end())
            return;

        // TODO : Use absolute file path
        // TODO : Use . for folders

        // Set this module to parsed
        parsedModules.insert(mod);

        // Special module that gathers multiple modules
        if (mod.name == "libc")
        {
            includes.insert("stdio.h");
            includes.insert("stdlib.h");
            includes.insert("math.h");
        }
        // Up module
        else if (mod.up)
        {
            mod.name += ".up";
            toParseModules.push({ mod, INFO });
        }
        // C module
        else
            // Add the extension
            includes.insert(mod.path() + ".h");
    }
    
    void Compiler::addFunction(Function *f)
    {
        // TODO : Test whether f is already in functions

        functions.push_back(f);
    }

    Variable *Compiler::getVariable(const string &ID)
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

        // Process functions
        for (auto f : functions)
            f->process(this);

        // Don't stringify program if there are errors
        if (generationError)
            return;

        // Generate all functions
        for (size_t i = 1; i < functions.size(); ++i)
            if (!functions[i]->isCDef)
                program += functions[i]->toString() + "\n";

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

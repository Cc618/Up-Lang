#include "components.h"

#include <iostream>

#include "compiler.h"
#include "colors.h"

using namespace std;

namespace up
{
    std::string cType(const std::string &id)
    {
        if (id == "auto")
            // Empty = error
            return "";

        if (id == "num")
            return "float";

        // TODO : Custom up string type
        if (id == "str")
            return "const char*";

        if (id == "nil")
            return "void";

        if (id == "bool")
            return "uint8";

        return id;
    }

    std::vector<std::string> typeArgList(const std::vector<Expression*> &ARGS)
    {
        vector<string> args;

        for (auto arg : ARGS)
            args.push_back(cType(arg->type));

        return args;
    }

    Expression::Expression(const std::string &TYPE)
        : type(TYPE)
    {}

    bool Expression::compatibleType(const std::string &TYPE) const
    {
        return TYPE == "auto"
            || TYPE == type;
    }

    ExpressionStatement::ExpressionStatement(const ErrorInfo &INFO, Expression *expr)
        : Statement(INFO), expr(expr)
    {}

    ExpressionStatement::~ExpressionStatement()
    {
        delete expr;
    }

    std::string ExpressionStatement::toString() const
    {
        // Just terminate the instruction
        return expr->toString() + ";";
    }

    void ExpressionStatement::process(Compiler *compiler)
    {
        expr->process(compiler);
    }

    std::string Literal::toString() const
    {
        if (type == "bool")
            return data == "yes" ? "true" : "false";

        if (type == "num")
            return data + "f";

        if (type == "str")
            // Replace the single quotes by double quotes
            return '"' + data.substr(1, data.size() - 2) + '"';

        return data;
    }

    std::string VariableUse::toString() const
    {
        return id;
    }

    Call::~Call()
    {
        for (auto a : args)
            delete a;
    }

    std::string Call::toString() const
    {
        string s = id;

        s += "(";

        if (!args.empty())
        {
            s += args[0]->toString();

            for (size_t i = 1; i < args.size(); ++i)
                s += ", " + args[i]->toString();
        }

        s += ")";

        return s;
    }

    void Call::process(Compiler *compiler)
    {
        // Check the function exists
        Function *func = compiler->getFunction(id, typeArgList(args));

        // Error : No function found
        if (!func)
        {
            string msg = "The function '";
            msg += id;
            msg += "' doesn't match any other function, verify the name or the arguments";

            compiler->generateError(msg, info);
        }

        // Update the return type
        type = func->type;
    }

    VariableDeclaration::VariableDeclaration(const ErrorInfo &INFO, const std::string &ID, const std::string &TYPE, Expression *expr)
        : Statement(INFO), id(ID), type(TYPE), expr(expr)
    {}

    VariableDeclaration::~VariableDeclaration()
    {
        if (expr)
            delete expr;
    }

    std::string VariableDeclaration::toString() const
    {
        if (expr)
            return parsedType + " " + id + " = " + expr->toString() + ";";
        else
            return parsedType + " " + id + ";";
    }

    void VariableDeclaration::process(Compiler *compiler)
    {
        // TODO : Check variable exists

        if (expr)
        {
            // Verify type compatibility
            if (expr->compatibleType(type))
                type = expr->type;
            else
            {
                compiler->generateError("Error for variable '" + id + "'\n" \
                    "Literal initialization must match the type of the variable\n", info);

                return;
            }
        }

        parsedType = cType(type);

        if (parsedType.empty())
            compiler->generateError(std::string("Error for variable '") + BLUE + id + DEFAULT +
                "'\nThe auto type can't be used in this context\n", info);
    }

    VariableAssignement::VariableAssignement(const ErrorInfo &INFO, const std::string &ID, Expression *expr, const std::string &OP)
        : Statement(INFO), id(ID), expr(expr), operand(OP)
    {
        // TODO : Check variable exists + expr and variable types are compatible
    }

    VariableAssignement::~VariableAssignement()
    {
        delete expr;
    }

    std::string VariableAssignement::toString() const
    {
        return id + " " + operand + " " + expr->toString() + ";";
    }

    Return::Return(const ErrorInfo &INFO, Expression *expr)
        : Statement(INFO), expr(expr)
    {}

    Return::~Return()
    {
        if (expr)
            delete expr;
    }

    string Return::toString() const
    {
        if (expr)
            return "return " + expr->toString() + ";";
        else
            return "return;";
    }

    void Return::process(Compiler *compiler)
    {
        // TODO : Set block type etc...
    }

    UnaryOperation::UnaryOperation(const std::string &ID, const std::string &OP, const bool PREFIX)
        : Expression("auto"), id(ID), operand(OP), prefix(PREFIX)
    {
        // TODO : Check variable exists + deduce type compatibility
    }

    std::string UnaryOperation::toString() const
    {
        if (prefix)
            return operand + id;
        else
            return id + operand;
    }

    Block::~Block()
    {
        for (auto instr : content)
            delete instr;
    }

    string Block::toString() const
    {
        string s = "{\n";

        // Add statements
        for (auto instr : content)
            s += "\t" + instr->toString() + "\n";

        s += "}\n";

        return s;
    }

    void Block::process(Compiler *compiler)
    {
        for (auto instr : content)
            instr->process(compiler);
    }

    Argument *Argument::createEllipsis(const ErrorInfo &INFO)
    {
        return new Argument(INFO, "...", "...");
    }

    Argument::Argument(const ErrorInfo &INFO, const string &TYPE, const string &ID)
        : ISyntax(INFO), type(TYPE), id(ID)
    {}

    string Argument::toString() const
    {
        return type + " " + id;
    }

    void Argument::process(Compiler *compiler)
    {
        // TODO : Check type (exists)
    }
    
    bool Argument::operator==(const Argument &ARG) const
    {
        return type == "..." || ARG.type == "..." || ARG.type == type;
    }

    Function::Function(const ErrorInfo &INFO, const std::string &TYPE, const std::string &ID, const vector<Argument*> &ARGS, const bool IS_C_DEF)
        : ISyntax(INFO), type(TYPE), id(ID), args(ARGS), isCDef(IS_C_DEF)
    {}

    Function::~Function()
    {
        for (auto arg : args)
            delete arg;
    }

    void Function::process(Compiler *compiler)
    {
        for (auto arg : args)
            arg->process(compiler);

        // TODO : Verify return type (exists)
    }

    string Function::signature() const
    {
        string s = cType(type) + " " + cName() + "(";

        if (!args.empty())
        {
            s += args[0]->toString();
            for (size_t i = 1; i < args.size(); ++i)
                s += ", " + args[i]->toString();
        }

        s += ")";

        return s;
    }

    UpFunction *UpFunction::createMain()
    {
        auto info = ErrorInfo(Module("main.c"), 0, 0);

        UpFunction *main = new UpFunction(info, "int", "main",
            { new Argument(info, "int", "argc"), new Argument(info, "char**", "argv") },
            new Block(info));

        return main;
    }

    UpFunction::UpFunction(const ErrorInfo &INFO, const std::string &TYPE, const std::string &ID, const vector<Argument*> &ARGS, Block *body)
        : Function(INFO, TYPE, ID, ARGS, false), body(body)
    {}

    UpFunction::~UpFunction()
    {
        delete body;
    }

    string UpFunction::toString() const
    {
        // Signature
        string s = signature() + " ";

        // Content
        s += body->toString();

        return s;
    }

    void UpFunction::process(Compiler *compiler)
    {
        Function::process(compiler);

        body->process(compiler);
    }

} // namespace up

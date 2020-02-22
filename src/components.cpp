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

    bool Call::isDeclarationList(Compiler *compiler) const
    {
        // TODO : Ellipsis
        // TODO : Named arg
        for (auto expr : args)
        {
            if (dynamic_cast<VariableUse*>(expr))
                continue;
         
            return false;
        }
        
        return true;
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

    CDef::CDef(const ErrorInfo &INFO, const std::string &TYPE, Call *call)
        : Statement(INFO), type(TYPE), call(call)
    {}

    void CDef::process(Compiler *compiler)
    {
        // Verify args
        if (!call->isDeclarationList(compiler))
        {
            compiler->generateError("Invalid argument list", info);
            return;
        }

        // TODO : Add function
        cout << "C function, id : " << call->id << ", type : " << type << ", args : ";

        for (auto a : call->args)
            cout << a->toString() << " ";

        cout << endl;
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

    Function *Function::createMain()
    {
        Function *main = new Function(ErrorInfo("main.c", 0, 0), "int", "main");

        main->isCDef = false;
        main->body = new Block(ErrorInfo("main.c", 0, 0));

        return main;
    }

    Function::Function(const ErrorInfo &INFO, const std::string &TYPE, const std::string &NAME)
        : ISyntax(INFO), type(TYPE), name(NAME), isCDef(true), body(nullptr)
    {}

    Function::Function(const ErrorInfo &INFO, const std::string &TYPE, Call *call, Block *body)
        : ISyntax(INFO), type(TYPE), name(call->id), isCDef(false), body(body)
    {
        // TODO change args type to be type/id tuples
        // TODO in process
        for (auto a : call->args)
            args.push_back(a->toString());

        delete call;
    }

    Function::~Function()
    {
        if (body)
            delete body;
    }

    string Function::signature() const
    {
        string s = cType(type) + " " + cName() + "(";

        if (!args.empty())
        {
            s += args[0];
            for (size_t i = 1; i < args.size(); ++i)
                s += ", " + args[i];
        }

        s += ")";

        return s;
    }

    std::string Function::toString() const
    {
        if (isCDef)
            return "";
        
        // Signature
        string s = signature() + " ";

        // Content
        s += body->toString();

        return s;
    }

    void Function::process(Compiler *compiler)
    {
        body->process(compiler);

        // TODO : Verify return value
    }
} // namespace up

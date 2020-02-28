#include "components.h"

#include <iostream>

#include "compiler.h"
#include "colors.h"

using namespace std;

namespace up
{
    string cType(const string &id)
    {
        if (id == "auto")
            // Empty = error
            return "auto";

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

    std::vector<string> typeArgList(const std::vector<Expression*> &ARGS)
    {
        vector<string> args;

        for (auto arg : ARGS)
            args.push_back(cType(arg->type));

        return args;
    }

    Expression::Expression(const ErrorInfo &INFO, const string &TYPE)
        : ISyntax(INFO), type(TYPE)
    {}

    bool Expression::compatibleType(const string &TYPE) const
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

    string ExpressionStatement::toString() const
    {
        // Just terminate the instruction
        return expr->toString() + ";";
    }

    void ExpressionStatement::process(Compiler *compiler)
    {
        expr->process(compiler);
    }

    ControlStatement::ControlStatement(const ErrorInfo &INFO, Expression *condition, Block *content, const string &KEYWORD)
        : Statement(INFO), condition(condition), content(content), keyword(KEYWORD)
    {}

    ControlStatement::~ControlStatement()
    {
        delete condition;
        delete content;
    }

    string ControlStatement::toString() const
    {
        string s = keyword;
        s += " (";
        s += condition->toString();
        s += ") ";
        s += content->toString();

        return s;
    }

    void ControlStatement::process(Compiler *compiler)
    {
        // TODO : check expr as type bool
        condition->process(compiler);
        content->process(compiler);
    }

    ConditionSequence::ConditionSequence(const ErrorInfo &INFO, ControlStatement *ifStmt)
        : Statement(INFO)
    {
        controls.push_back(ifStmt);
    }

    ConditionSequence::~ConditionSequence()
    {
        for (auto s : controls)
            delete s;
    }

    string ConditionSequence::toString() const
    {
        // Create block to avoid ambiguity
        string s = "{\n";

        for (auto c : controls)
            s += c->toString();

        s += "}\n";

        return s;
    }

    void ConditionSequence::process(Compiler *compiler)
    {
        int i = 0;
        for (auto s : controls)
        {
            // An or is always at the end
            if (dynamic_cast<OrStatement*>(s) && i != controls.size() - 1)
                compiler->generateError("An or statement must be at the end of a control sequence", info);

            s->process(compiler);

            ++i;
        }
    }

    OrStatement::OrStatement(const ErrorInfo &INFO, Block *content)
        : Statement(INFO), content(content)
    {}

    OrStatement::~OrStatement()
    {
        delete content;
    }

    string OrStatement::toString() const
    {
        return "else " + content->toString();
    }

    void OrStatement::process(Compiler *compiler)
    {
        content->process(compiler);
    }

    ForStatement *ForStatement::createDefaultInit(const ErrorInfo &INFO, const std::string &VAR_ID,
        Expression *end, Block *content)
    {
        return new ForStatement(INFO, VAR_ID, new Literal(INFO, "0", "int"), end, content);
    }

    ForStatement::ForStatement(const ErrorInfo &INFO, const string &VAR_ID, Expression *begin,
        Expression *end, Block *content)
        : Statement(INFO), varId(VAR_ID), begin(begin), end(end), content(content)
    {}

    ForStatement::~ForStatement()
    {
        delete begin;
        delete end;
    }

    string ForStatement::toString() const
    {
        string s = "for (int ";
        s += varId + " = " + begin->toString();
        s += "; " + varId + " < " + end->toString();
        s += "; ++" + varId;
        s += ") " + content->toString();

        return s; 
    }

    void ForStatement::process(Compiler *compiler)
    {
        // TODO : Add the variable to content's scope
        // TODO : Verify begin + end type (int)

        begin->process(compiler);
        end->process(compiler);
    }

    string Literal::toString() const
    {
        if (type == "bool")
            // Use 1 or 0, not true or false
            return data == "yes" ? "1" : "0";

        if (type == "num")
            return data + "f";

        if (type == "str")
            // Replace the single quotes by double quotes
            return '"' + data.substr(1, data.size() - 2) + '"';

        return data;
    }

    string VariableUse::toString() const
    {
        return id;
    }

    Call::~Call()
    {
        for (auto a : args)
            delete a;
    }

    string Call::toString() const
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

    VariableDeclaration::VariableDeclaration(const ErrorInfo &INFO, const string &ID, const string &TYPE, Expression *expr)
        : Statement(INFO), id(ID), type(TYPE), expr(expr)
    {}

    VariableDeclaration::~VariableDeclaration()
    {
        if (expr)
            delete expr;
    }

    string VariableDeclaration::toString() const
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
            expr->process(compiler);

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
            compiler->generateError(string("Error for variable '") + BLUE + id + DEFAULT +
                "'\nThe auto type can't be used in this context\n", info);
    }

    VariableAssignement::VariableAssignement(const ErrorInfo &INFO, const string &ID, Expression *expr, const string &OP)
        : Statement(INFO), id(ID), expr(expr), operand(OP)
    {
        // TODO : Check variable exists + expr and variable types are compatible
    }

    VariableAssignement::~VariableAssignement()
    {
        delete expr;
    }

    string VariableAssignement::toString() const
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

        if (expr)
            expr->process(compiler);
    }

    UnaryOperation::UnaryOperation(const ErrorInfo &INFO, const string &ID, const string &OP, const bool PREFIX)
        : Expression(INFO, "auto"), id(ID), operand(OP), prefix(PREFIX)
    {
        // TODO : Check variable exists + deduce type compatibility
    }

    string UnaryOperation::toString() const
    {
        if (prefix)
            return operand + id;
        else
            return id + operand;
    }


    BinaryOperation::BinaryOperation(const ErrorInfo &INFO, Expression *first, Expression *second, const string &OP, const bool COND)
        : Expression(INFO, COND ? "bool" : "auto"), first(first), second(second), operand(OP), condition(COND)
    {}

    BinaryOperation::~BinaryOperation()
    {
        delete first;
        delete second;
    }

    string BinaryOperation::toString() const
    {
        return first->toString() + " " + operand + " " + second->toString();
    }

    void BinaryOperation::process(Compiler *compiler)
    {
        // TODO : Deduce type compatibility (casts), if (condition)
        first->process(compiler);
        second->process(compiler);
    }

    Block *Block::createOrStatement(const ErrorInfo &IF_INFO, const ErrorInfo &OR_INFO,
        Expression *condition, Block *ifContent, Block *orContent)
    {
        Block *b = new Block(IF_INFO);

        b->content.push_back(new ControlStatement(IF_INFO, condition, ifContent, "if"));
        b->content.push_back(new OrStatement(OR_INFO, orContent));

        return b;
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

    Function::Function(const ErrorInfo &INFO, const string &TYPE, const string &ID, const vector<Argument*> &ARGS, const bool IS_C_DEF)
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

    UpFunction::UpFunction(const ErrorInfo &INFO, const string &TYPE, const string &ID, const vector<Argument*> &ARGS, Block *body)
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

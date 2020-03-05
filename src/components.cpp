#include "components.h"

#include <iostream>

#include "compiler.h"
#include "types.h"
#include "colors.h"

using namespace std;

namespace up
{
    Expression::Expression(const ErrorInfo &INFO, const Id &TYPE)
        : ISyntax(INFO), type(TYPE)
    {}

    bool Expression::compatibleType(const Id &TYPE) const
    {
        // TODO Change func by global one
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


    CStatement::CStatement(const ErrorInfo &INFO, const string &CODE)
        : Statement(INFO), code(CODE)
    {}

    string CStatement::toString() const
    {
        return "\n" + code + "\n";
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
        condition->process(compiler);
        content->process(compiler);

        if (condition->type != "bool")
        {
            compiler->generateError("The condition must have type '" + AS_BLUE("bool") + "' but has type '" +
                AS_BLUE(condition->type.toUp()) + "'", info);
            return;
        }
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
        string s;

        for (auto c : controls)
            s += c->toString();

        s += "\n";

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

    ForStatement *ForStatement::createDefaultInit(const ErrorInfo &INFO, const Id &VAR_ID,
        Expression *end, Block *content)
    {
        return new ForStatement(INFO, VAR_ID, new Literal(INFO, "0", Id("int")), end, content);
    }

    ForStatement::ForStatement(const ErrorInfo &INFO, const Id &VAR_ID, Expression *begin,
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
        s += varId.name() + " = " + begin->toString();
        s += "; " + varId.name() + " < " + end->toString();
        s += "; ++" + varId.name();
        s += ") " + content->toString();

        return s;
    }

    void ForStatement::process(Compiler *compiler)
    {
        // The iterator must have a simple id
        if (!varId.isSimple())
        {
            compiler->generateError("The iterator '" +
                AS_BLUE(varId.toUp()) + "' must have a simple id (no prefix)", info);
            return;
        }

        // TODO : Update target type with num support
        string targetType = "int";
        
        // Add the variable to the content's scope
        content->vars.push_back(new Variable(varId, targetType));

        if (!begin->compatibleType(targetType))
        {
            compiler->generateError("The begin expression of the for statement must have '" +
                AS_BLUE(targetType) + "' type but has '" + AS_BLUE(begin->type.toUp()) + "' type", info);
            return;
        }

        if (!end->compatibleType(targetType))
        {
            compiler->generateError("The end expression of the for statement must have '" +
                AS_BLUE(targetType) + "' type but has '" + AS_BLUE(end->type.toUp()) + "' type", info);
            return;
        }

        begin->process(compiler);
        end->process(compiler);
        content->process(compiler);
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

    string VariableUsage::toString() const
    {
        // TODO : Better mangling
        return id.toC();
    }

    void VariableUsage::process(Compiler *compiler)
    {
        // Check exists
        if (Variable *v = compiler->getVar(id))
            // Retrieve type
            type = v->type;
        else
            // Error
            compiler->generateError("The variable named '" + AS_BLUE(id.toUp()) + "' is not declared in this scope", info);
    }

    Call::~Call()
    {
        for (auto a : args)
            delete a;
    }

    string Call::toString() const
    {
        // TODO : Better mangling
        string s = id.toC();

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
            compiler->generateError("The function '" + AS_BLUE(id.toUp()) +
                + "' doesn't match any other function, verify the name or the arguments", info);
            return;
        }

        // Update the return type
        type = func->type;
    }

    VariableDeclaration::VariableDeclaration(const ErrorInfo &INFO, const Id &ID, const Id &TYPE, Expression *expr)
        : Statement(INFO), id(ID), type(TYPE), expr(expr)
    {}

    VariableDeclaration::~VariableDeclaration()
    {
        if (expr)
            delete expr;
    }

    string VariableDeclaration::toString() const
    {
        // TODO : Better mangling (*2)
        if (expr)
            return parsedType + " " + id.toC() + " = " + expr->toString() + ";";
        else
            return parsedType + " " + id.toC() + ";";
    }

    void VariableDeclaration::process(Compiler *compiler)
    {
        // Check simple id
        if (!id.isSimple())
        {
            compiler->generateError("The variable '" +
                AS_BLUE(id.toUp()) + "' must have a simple id (no prefix)", info);
            return;
        }

        // Check simple type
        if (!type.isSimple())
        {
            compiler->generateError("The type '" + AS_BLUE(type.toUp()) + "' of the variable '" +
                AS_BLUE(id.toUp()) + "' must have a simple name (no prefix)", info);
            return;
        }

        // Check type exists
        if (type != "auto" && !typeExists(type))
        {
            compiler->generateError("The type '" + AS_BLUE(type.toUp()) + "' of the variable '" +
                AS_BLUE(id.toUp()) + "' isn't declared", info);
            return;
        }

        // Error : The variable already exists in this scope
        if (compiler->scopes.back()->getVar(id))
        {
            compiler->generateError("The variable '" + AS_BLUE(id.toUp()) +
                "' already exists in this scope", info);
            return;
        }

        if (expr)
        {
            expr->process(compiler);

            // Verify type compatibility
            if (expr->compatibleType(type))
                type = expr->type;
            else
            {
                compiler->generateError("Error for variable '" + AS_BLUE(id.toUp()) + "'\n" \
                    "The initialization expression of type '" + AS_BLUE(expr->type.toUp()) +
                    "' must match the type of the variable\n", info);

                return;
            }
        }

        // TODO : Move cType in Id
        parsedType = cType(type.toUp());

        if (parsedType.empty())
        {
            compiler->generateError("Error for variable '" + AS_BLUE(id.toUp()) +
                "'\nThe auto type can't be used in this context", info);
            return;
        }

        // Push the variable in the scope
        compiler->scopes.back()->vars.push_back(new Variable(id, type));
    }

    VariableAssignement::VariableAssignement(const ErrorInfo &INFO, const Id &ID, Expression *expr, const string &OP)
        : Statement(INFO), id(ID), expr(expr), operand(OP)
    {}

    VariableAssignement::~VariableAssignement()
    {
        delete expr;
    }

    string VariableAssignement::toString() const
    {
        return id.toC() + " " + operand + " " + expr->toString() + ";";
    }

    void VariableAssignement::process(Compiler *compiler)
    {
        // Check simple id
        if (!id.isSimple())
        {
            compiler->generateError("The variable '" +
                AS_BLUE(id.toUp()) + "' must have a simple id (no prefix)", info);
            return;
        }

        // TODO : Constructors / Destructors

        // Error : The variable doesn't exists
        Variable *v = compiler->getVar(id);

        // Check exists
        if (!v)
        {
            compiler->generateError("The variable '" + AS_BLUE(id.toUp()) + "' exists already in this scope", info);
            return;
        }

        expr->process(compiler);

        // Check compatible types
        if (!expr->compatibleType(v->type))
        {
            compiler->generateError("The type '" + AS_BLUE(v->type.toUp()) + "' of the variable '" +
                AS_BLUE(id.toUp()) + "' is not compatible with the type '" + AS_BLUE(expr->type.toUp()) + "'", info);
            return;
        }

        // Check operator declared (or expression operator)
        if (operand.size() > 1)
        {
            const string OP = string(1, operand[0]);
            if (!operatorExists(expr->type, OP))
                compiler->generateError("The operator '" + AS_BLUE(operand) +
                    "' (or '" + AS_BLUE(OP) + "') can't be used with the type '" +
                    AS_BLUE(expr->type.toUp()) + "'",
                    info);
        }
        else if (!operatorExists(expr->type, operand))
            compiler->generateError("The operator '" + AS_BLUE(operand) +
                "' can't be used with the type '" + AS_BLUE(expr->type.toUp()) + "'",
                info);

        // TODO : Generate function for non builtin types
        // TODO : Generate the good function for +=...
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

    UnaryOperation::UnaryOperation(const ErrorInfo &INFO, const Id &ID, const string &OP, const bool PREFIX)
        : Expression(INFO, Id::createAuto()), id(ID), operand(OP), prefix(PREFIX)
    {}

    string UnaryOperation::toString() const
    {
        if (prefix)
            return operand + id.toC();
        else
            return id.toC() + operand;
    }

    void UnaryOperation::process(Compiler *compiler)
    {
        // Check simple id
        if (!id.isSimple())
        {
            compiler->generateError("The variable '" +
                AS_BLUE(id.toUp()) + "' must have a simple id (no prefix)", info);
            return;
        }

        Variable *v = compiler->getVar(id);

        // Check variable exists
        if (!v)
        {
            compiler->generateError("Variable '" + AS_BLUE(id.toUp()) + "' is not declared in this scope", info);
            return;
        }

        // Set type
        type = v->type;

        if (!operatorExists(type, operand))
            compiler->generateError("The operator '" + AS_BLUE(operand) +
                "' can't be used with the type '" + AS_BLUE(type.toUp()) + "'",
                info);

        // TODO : Call custom function for non builtin types
    }

    BinaryOperation::BinaryOperation(const ErrorInfo &INFO, Expression *first, Expression *second, const string &OP, const bool COND)
        : Expression(INFO, COND ? Id("bool") : Id::createAuto()), first(first), second(second), operand(OP), condition(COND)
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
        first->process(compiler);
        second->process(compiler);

        // Type compatibility
        if (!first->compatibleType(second->type))
            compiler->generateError("Types '" + AS_BLUE(first->type.toUp()) +
                "' and '" + AS_BLUE(second->type.toUp()) + "' are incompatible for '" +
                AS_BLUE(operand) + "' operation", info);

        // Check operator declared
        if (!operatorExists(first->type, operand))
            compiler->generateError("The operator '" + AS_BLUE(operand) +
                "' can't be used with the type '" + AS_BLUE(first->type.toUp()) + "'",
                info);

        // TODO : Call custom function for non builtin types

        // Set type
        if (condition)
            type = Id("bool");
        else
            type = first->type;
    }

    Block::~Block()
    {
        for (auto instr : content)
            delete instr;

        for (auto v : vars)
            delete v;
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
        // Push scope
        compiler->scopes.push_back(this);

        for (auto instr : content)
            instr->process(compiler);

        compiler->scopes.pop_back();
    }

    Variable *Block::getVar(const Id &ID)
    {
        for (auto v : vars)
            if (v->id == ID)
                return v;

        return nullptr;
    }

    Argument *Argument::createEllipsis(const ErrorInfo &INFO)
    {
        return new Argument(INFO, Id::createEllipsis(), Id::createEllipsis());
    }

    Argument::Argument(const ErrorInfo &INFO, const Id &TYPE, const Id &ID)
        : ISyntax(INFO), type(TYPE), id(ID)
    {}

    string Argument::toString() const
    {
        // TODO : cType change
        return cType(type.toUp()) + " " + id.toC();
    }

    void Argument::process(Compiler *compiler)
    {
        // Check type exists
        if (!typeExists(type))
        {
            compiler->generateError("The type '" + AS_BLUE(type.toUp()) + "' of the argument '" +
                AS_BLUE(id.toUp()) + "' isn't declared", info);
            return;
        }

        // Check simple id
        if (!id.isSimple())
            compiler->generateError("The variable '" +
                AS_BLUE(id.toUp()) + "' must have a simple id (no prefix)", info);

        // Check simple type
        if (!type.isSimple())
            compiler->generateError("The type '" + AS_BLUE(type.toUp()) + "' of the variable '" +
                AS_BLUE(id.toUp()) + "' must have a simple id (no prefix)", info);
    }

    bool Argument::operator==(const Argument &ARG) const
    {
        return type == "..." || ARG.type == "..." || ARG.type == type;
    }

    Function::Function(const ErrorInfo &INFO, const Id &TYPE, const Id &ID, const vector<Argument*> &ARGS, const bool IS_C_DEF)
        : ISyntax(INFO), type(TYPE), id(ID), args(ARGS), isCDef(IS_C_DEF)
    {}

    Function::~Function()
    {
        for (auto arg : args)
            delete arg;
    }

    void Function::process(Compiler *compiler)
    {
        // Check simple type
        if (!type.isSimple())
        {
            compiler->generateError("The type '" + AS_BLUE(type.toUp()) + "' of the function '" +
                AS_BLUE(id.toUp()) + "' must have a simple id (no prefix)", info);
            return;
        }

        // Check type exists
        if (!typeExists(type))
        {
            compiler->generateError("The type '" + AS_BLUE(type.toUp()) + "' of the function '" +
                AS_BLUE(id.toUp()) + "' isn't declared", info);
            return;
        }

        for (auto arg : args)
            arg->process(compiler);

        // TODO : Verify return type (exists)
    }

    string Function::signature() const
    {
        // TODO : cType
        string s = cType(type.toUp()) + " " + cName() + "(";

        if (!args.empty())
        {
            s += args[0]->toString();
            for (size_t i = 1; i < args.size(); ++i)
                s += ", " + args[i]->toString();
        }

        s += ")";

        return s;
    }

    bool Function::operator==(const Function &OTHER) const
    {
        // TODO : Add type check also when function overloading
        return id == OTHER.id;
    }

    UpFunction *UpFunction::createMain()
    {
        auto info = ErrorInfo(Module(Id("main.c")), 0, 0);

        UpFunction *main = new UpFunction(info, Id("int"), Id("main"),
            { new Argument(info, Id("int"), Id("argc")), new Argument(info, Id("char**"), Id("argv")) },
            new Block(info));

        return main;
    }

    UpFunction::UpFunction(const ErrorInfo &INFO, const Id &TYPE, const Id &ID, const vector<Argument*> &ARGS, Block *body)
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

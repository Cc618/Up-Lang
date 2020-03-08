#pragma once

// All classes parsed and then compiled

#include <string>
#include <vector>
#include <iostream>

#include "error_info.h"
#include "variable.h"
#include "id.h"

namespace up
{
    class Compiler;
    class Expression;
    class Block;

    // Interface which provides process and toString virtual functions
    class ISyntax
    {
    public:
        ISyntax() = default;
        ISyntax(const ErrorInfo &INFO)
            : info(INFO)
        {}
        virtual ~ISyntax() = default;

    public:
        // Returns the string representation of
        // the syntax
        virtual std::string toString() const
        { return ""; }

        // Like a constructor with the compiler as argument
        virtual void process(Compiler *compiler)
        {}

    public:
        // To obtain details when there is an error (file, location...)
        ErrorInfo info;
    };

    // Gives a result
    // For example :
    // 42 + 618
    // * The type must be resolved when process is called
    class Expression : public ISyntax
    {
    public:
        Expression() = default;
        Expression(const ErrorInfo &INFO, const Id &TYPE);
        virtual ~Expression() = default;

    public:
        // Whether the type is compatible to this type
        // * TYPE belongs to a Variable
        // !!! TYPE is a Up type
        bool compatibleType(const Id &TYPE) const;

    public:
        // Up type (can be auto)
        Id type;
    };

    // A statement is like an instruction but
    // can be also a sequence of instructions
    // For example :
    // $a = 48
    class Statement : public ISyntax
    {
    public:
        Statement() = default;
        Statement(const ErrorInfo &INFO)
            : ISyntax(INFO)
        {}
        virtual ~Statement() = default;
    };

    // Used to convert expression to statement
    // This is an expression used as a statement,
    // aka inline expression (return value ignored)
    // For example :
    // a++;
    class ExpressionStatement : public Statement
    {
    public:
        ExpressionStatement() = default;
        ExpressionStatement(const ErrorInfo &INFO, Expression *expr);
        ~ExpressionStatement();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    private:
        // The expression which inits the variable
        Expression *expr;
    };

    // A C section within a block
    // For example :
    // %{ int myNb = 42; %}
    class CStatement : public Statement
    {
    public:
        CStatement() = default;
        CStatement(const ErrorInfo &INFO, const std::string &CODE);

    public:
        virtual std::string toString() const override;

    private:
        std::string code;
    };

    // When a statement contains one or more blocks
    class IBlockStatement : public Statement
    {
    public:
        IBlockStatement() = default;
        IBlockStatement(const ErrorInfo &INFO)
            : Statement(INFO)
        {}
        virtual ~IBlockStatement()
        {}

    public:
        virtual void pushDestructor(const std::string &des) = 0;
    };

    // When a statement contains only one block
    class IMonoBlockStatement : public IBlockStatement
    {
    public:
        IMonoBlockStatement() = default;
        IMonoBlockStatement(const ErrorInfo &INFO, Block *content);
        virtual ~IMonoBlockStatement();

    public:
        virtual void pushDestructor(const std::string &DES) override;
    
    protected:
        Block *content;
    };

    // An if or while
    // For example :
    // condition ?
    //      block
    // if : KEYWORD
    class ControlStatement : public IMonoBlockStatement
    {
    public:
        ControlStatement() = default;
        ControlStatement(const ErrorInfo &INFO, Expression *condition, Block *content,
            const std::string &KEYWORD);
        ~ControlStatement();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    private:
        Expression *condition;
        std::string keyword;
    };

    // Sequence of if / or if / or
    // * Starts with a if
    class ConditionSequence : public IBlockStatement
    {
    public:
        ConditionSequence() = default;
        ConditionSequence(const ErrorInfo &INFO, ControlStatement *ifStmt);
        ~ConditionSequence();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        virtual void pushDestructor(const std::string &DES) override;

    public:
        // If / or if / or block statements
        std::vector<Statement*> controls;
    };

    // The or block
    // For example :
    // or
    //      block
    // !!! Used only in Block::createOrStatement
    class OrStatement : public IMonoBlockStatement
    {
    public:
        OrStatement() = default;
        OrStatement(const ErrorInfo &INFO, Block *content);
        ~OrStatement();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;
    };

    // A for loop
    // For example :
    // for i to 42
    //      block
    // i : VAR_ID, 0 : begin, 42 : end
    class ForStatement : public IMonoBlockStatement
    {
    public:
        // Creates a for statement with the default initializer (begin)
        static ForStatement *createDefaultInit(const ErrorInfo &INFO, const Id &VAR_ID,
            Expression *end, Block *content);

    public:
        ForStatement() = default;
        ForStatement(const ErrorInfo &INFO, const Id &VAR_ID, Expression *begin,
            Expression *end, Block *content);
        ~ForStatement();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    private:
        Expression *begin;
        Expression *end;
        Id varId;
    };

    // A literal expression
    // Matches a type
    // For example :
    // 42
    class Literal : public Expression
    {
    public:
        Literal() = default;
        Literal(const ErrorInfo &INFO, const std::string &DATA, const Id &TYPE)
            : Expression(INFO, TYPE), data(DATA)
        {}

    public:
        virtual std::string toString() const override;

    private:
        std::string data;
    };

    // When we use a variable in a statement
    // For example :
    // int a = b
    // Variable use : b (ID)
    class VariableUsage : public Expression
    {
    public:
        VariableUsage() = default;
        VariableUsage(const ErrorInfo &INFO, const Id &ID)
            : Expression(INFO, Id::createAuto()), id(ID)
        {}

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    private:
        Id id;
    };

    // A function call with arg list
    // Matches a type
    // For example :
    // fun(a1, a2)
    // fun : ID, { a1, a2 } : args
    // * It is not an argument list to declare functions
    class Call : public Expression
    {
    public:
        Call() = default;
        Call(const ErrorInfo &INFO, const Id &ID, const std::vector<Expression*> &ARGS={}, const bool IS_DESTRUCTOR=false)
            : Expression(INFO, Id::createAuto()), id(ID), args(ARGS), isDestructor(IS_DESTRUCTOR)
        {}
        ~Call();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        Id id;
        std::vector<Expression*> args;
        bool isDestructor;
    };

    // For example :
    // $a = 6
    class VariableDeclaration : public Statement
    {
    public:
        VariableDeclaration() = default;
        // expr can be nullptr if the variable is not init
        VariableDeclaration(const ErrorInfo &INFO, const Id &ID, const Id &TYPE, Expression *expr);
        ~VariableDeclaration();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        // TODO : Remove parsedType (generate only in toString) ?
        // C type
        std::string parsedType;
        Id type;
        Id id;

    private:
        // The expression which inits the variable
        Expression *expr;
    };

    // For example :
    // a += 6
    // a : ID, + : OPERAND, 6 : expr
    class VariableAssignement : public Statement
    {
    public:
        VariableAssignement() = default;
        VariableAssignement(const ErrorInfo &INFO, const Id &ID, Expression *expr, const std::string &OPERAND);
        ~VariableAssignement();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        Id id;

    private:
        // The expression which modifies the variable
        Expression *expr;
        std::string operand;
    };

    // For example :
    // ret 3.14
    class Return : public Statement
    {
    public:
        Return() = default;
        // expr can be nullptr if the return is null
        Return(const ErrorInfo &INFO, Expression *expr);
        ~Return();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    private:
        // The expression which inits the variable
        Expression *expr;
    };

    // For example :
    // a++
    // a : ID, ++ : OPERAND
    class UnaryOperation : public Expression
    {
    public:
        UnaryOperation() = default;
        UnaryOperation(const ErrorInfo &INFO, const Id &ID, const std::string &OPERAND, const bool PREFIX=false);

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        Id id;

    private:
        std::string operand;
        bool prefix;
    };

    // For example :
    // a + 42
    // a : first, 42 : second, + : OPERAND
    class BinaryOperation : public Expression
    {
    public:
        BinaryOperation() = default;
        // If CONDITION, the type is bool
        BinaryOperation(const ErrorInfo &INFO, Expression *first, Expression *second, const std::string &OPERAND, const bool CONDITION=false);
        ~BinaryOperation();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    private:
        std::string operand;
        Expression *first;
        Expression *second;
        // If boolean condition
        bool condition;
    };

    // A block is like the body of a function or a if statement
    // This gathers indented statements and other instructions
    class Block : public ISyntax
    {
    public:
        Block() = default;
        Block(const ErrorInfo &INFO)
            : ISyntax(INFO)
        {}
        ~Block();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        // !!! Can return nullptr
        Variable *getVar(const Id &ID);

        // Adds a statement in the content        
        void pushStatement(Statement *s);

    public:
        std::vector<Variable*> vars;

        // All destructors as string
        std::string destructors;
    
    private:
        // The content
        std::vector<Statement*> content;
    };

    // An argument within a function definition
    // For example :
    // int main(int argc)
    // int argc is an argument
    // int : TYPE, argc : ID
    class Argument : public ISyntax
    {
    public:
        // Returns the argument for ...
        static Argument *createEllipsis(const ErrorInfo &INFO);

    public:
        Argument() = default;
        Argument(const ErrorInfo &INFO, const Id &TYPE, const Id &ID);

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

        inline bool isEllipsis() const
        { return type == "..."; }

    public:
        // Compares two args (only types)
        // * Returns true if one argument is the ellipsis argument
        bool operator==(const Argument &ARG) const;

    public:
        // The up type and id
        Id type,
            id;
    };

    // Describes a function, can be a cdef or updef
    // For updef, use UpFunction
    class Function : public ISyntax
    {
    public:
        // Creates a cdef function
        static Function *createCDef(const ErrorInfo &INFO, const Id &TYPE, const Id &ID,
            const std::vector<Argument*> &ARGS);

    public:
        Function() = default;
        Function(const ErrorInfo &INFO, const Id &TYPE, const Id &ID,
            const std::vector<Argument*> &ARGS, const bool IS_C_DEF);
        virtual ~Function();

    public:
        virtual void process(Compiler *compiler) override;

    public:
        // The c signature (without ;)
        std::string signature() const;
        // Returns the mangled name
        std::string cName() const
        {
            // TODO : Name mangling with args, ...
            // TODO : Better mangling
            return id.toC();
        }

    public:
        bool operator==(const Function &OTHER) const;

    public:
        // To check whether this function is defined in c or up
        bool isCDef;
        // Whether this is an object's method
        bool isMethod = false;
        bool isDestructor = false;
        std::vector<Argument*> args;
        // Return type
        Id type;
        // Name
        Id id;
    };

    // A function defined in up
    // (Function with instructions within and string representation)
    class UpFunction : public Function
    {
    public:
        // Returns the main function
        static UpFunction *createMain();

    public:
        UpFunction() = default;
        UpFunction(const ErrorInfo &INFO, const Id &TYPE, const Id &ID,
            const std::vector<Argument*> &ARGS, Block *body);
        ~UpFunction();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        // Instructions
        Block *body;
    };
}

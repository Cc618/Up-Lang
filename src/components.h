#pragma once

// All classes parsed and then compiled

#include <string>
#include <vector>

#include "error_info.h"

namespace up
{
    class Compiler;
    class Expression;

    // Casts up type to c type in string
    // !!! May return empty string if the type
    // !!! is an auto type
    std::string cType(const std::string &TYPE);

    // Returns the argument types list
    // All arguments have a c type
    std::vector<std::string> typeArgList(const std::vector<Expression*> &ARGS);


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
        Expression(const std::string &TYPE);
        virtual ~Expression() = default;

    public:
        // Whether the type is compatible to this type
        // * TYPE belongs to a Variable
        // !!! TYPE is a Up type
        bool compatibleType(const std::string &TYPE) const;

    public:
        // Up type (can be auto)
        std::string type;
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

    // A literal expression
    // Matches a type
    // For example :
    // 42
    class Literal : public Expression
    {
    public:
        Literal() = default;
        Literal(const std::string &DATA, const std::string &TYPE)
            : Expression(TYPE), data(DATA)
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
    class VariableUse : public Expression
    {
    public:
        VariableUse() = default;
        VariableUse(const std::string &ID)
            // TODO : Type
            : Expression("auto"), id(ID)
        {}

    public:
        virtual std::string toString() const override;

    private:
        std::string id;
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
        Call(const std::string &ID, const std::vector<Expression*> &ARGS={})
            // TODO : decl type of the function
            : Expression("auto"), id(ID), args(ARGS)
        {}
        ~Call();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        std::string id;
        std::vector<Expression*> args;
    };

    // For example :
    // $a = 6
    class VariableDeclaration : public Statement
    {
    public:
        VariableDeclaration() = default;
        // expr can be nullptr if the variable is not init
        VariableDeclaration(const ErrorInfo &INFO, const std::string &ID, const std::string &TYPE, Expression *expr);
        ~VariableDeclaration();

    public:
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        // C type
        std::string parsedType;
        std::string type;
        std::string id;

    private:
        // The expression which inits the variable
        Expression *expr;
    };

    // For example :
    // $a += 6
    // a : ID, + : OPERAND, 6 : expr
    class VariableAssignement : public Statement
    {
    public:
        VariableAssignement() = default;
        VariableAssignement(const ErrorInfo &INFO, const std::string &ID, Expression *expr, const std::string &OPERAND);
        ~VariableAssignement();

    public:
        virtual std::string toString() const override;

    public:
        std::string id;

    private:
        // The expression which modifies the variable
        Expression *expr;
        std::string operand;
    };

    // For example :
    // a++
    // a : ID, ++ : OPERAND
    class UnaryOperation : public Expression
    {
    public:
        UnaryOperation() = default;
        UnaryOperation(const std::string &ID, const std::string &OPERAND, const bool PREFIX=false);

    public:
        virtual std::string toString() const override;

    public:
        std::string id;

    private:
        std::string operand;
        bool prefix;
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
        Argument(const ErrorInfo &INFO, const std::string &TYPE, const std::string &ID);

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
        std::string type,
            id;
    };

    // Describes a function, can be a cdef or updef
    // For updef, use UpFunction
    class Function : public ISyntax
    {

    public:
        Function() = default;
        Function(const ErrorInfo &INFO, const std::string &TYPE, const std::string &ID,
            const std::vector<Argument*> &ARGS, const bool IS_C_DEF=true);
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
            return id;
        }

    public:
        // To check whether this function is defined in c or up
        bool isCDef;
        std::vector<Argument*> args;
        // Return type
        std::string type;
        // Name
        std::string id;
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
        UpFunction(const ErrorInfo &INFO, const std::string &TYPE, const std::string &ID,
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

#pragma once

// All classes parsed and then compiled

#include <string>
#include <vector>

#include "error_info.h"

namespace up
{
    class Compiler;

    // Casts up type to c type in string
    // !!! May return empty string if the type
    // !!! is an auto type
    std::string cType(const std::string &TYPE);


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

        // Whether the args are an argument declaration list
        // * Can contain ellipsis
        // !!! Can raise error if the args are both types (ellipsis and operations)
        bool isDeclarationList(Compiler *compiler) const;

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

    // For example :
    // cdef num cosf(num)
    // TYPE : num, ID : cosf, args : { num }
    // TODO : Statement ? Not like import (compiler.import)
    class CDef : public Statement
    {
    public:
        CDef() = default;
        // * TYPE can be auto
        // TODO : * call.args might have ellipsis type at the end ("...")
        CDef(const ErrorInfo &INFO, const std::string &TYPE, Call *call);
        ~CDef()
        { delete call; }

    public:
        virtual std::string toString() const
        { return ""; }
        virtual void process(Compiler *compiler) override;

    public:
        std::string type;
        Call *call;
    };

    // A block is like the body of a function or a if statement
    // This gathers indented statements
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
        // TODO : Change with ISyntax (gathers blocks, statements...)
        std::vector<Statement*> statements;
    };
   
    // A function describes a function with
    // instructions or a function declaration (cdef)
    class Function : public ISyntax
    {
    public:
        // Returns the main function
        static Function *createMain();

    public:
        Function() = default;
        // For cdefs
        Function(const ErrorInfo &INFO, const std::string &TYPE, const std::string &NAME);
        // For up defs
        Function(const ErrorInfo &INFO, const std::string &TYPE, Call *call, Block *content);
        ~Function();

    public:
        // Getter for name
        inline std::string upName() const
        { return name; }
        // Returns the mangled name
        std::string cName() const
        {
            // TODO : Name mangling with args, ...
            return upName();
        }

        // Returns the signature of the function
        // For example :
        // int main(int, char**)
        // !!! ; is not added at the end of the line
        std::string signature() const;

        // If isCDef, the function will return an empty string
        // Otherwise : Returns the function with its signature
        // and all statements
        virtual std::string toString() const override;
        virtual void process(Compiler *compiler) override;

    public:
        // Can be auto if and only if isCDef
        std::string type;
        bool isCDef;

        // The content
        Block *content;

    private:
        // Base name
        std::string name;
        // Args (only type names)
        std::vector<std::string> args;
    };

}

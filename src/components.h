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

    // Gives a result
    // For example :
    // 42 + 618
    // * The type must be resolved when process is called
    class Expression
    {
    public:
        Expression() = default;
        Expression(const std::string &TYPE);
        virtual ~Expression() = default;

    public:
        // Returns the string representation of
        // the expression in C
        virtual std::string toString() const = 0;

        // Whether the type is compatible to this type
        // * TYPE belongs to a Variable
        // !!! TYPE is a Up type
        bool compatibleType(const std::string &TYPE) const;
        
        // Like a constructor with the compiler as argument
        virtual void process(Compiler *compiler)
        {}

    public:
        // Up type (can be auto)
        std::string type;
    };

    // A statement is like an instruction but
    // can be also a sequence of instructions
    // For example :
    // $a = 48
    class Statement
    {
    public:
        Statement() = default;
        Statement(const ErrorInfo &INFO)
            : info(INFO)
        {}
        virtual ~Statement() = default;

    public:
        // Returns the string representation of
        // the statement in C
        virtual std::string toString() const = 0;

        // Like a constructor with the compiler as argument
        virtual void process(Compiler *compiler)
        {}
    
    public:
        ErrorInfo info;
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
        Call(const std::string &ID)
            // TODO : decl type of the function
            : Expression("auto"), id(ID)
        {}
        ~Call();

    public:
        virtual std::string toString() const override;

    public:
        std::vector<Expression*> args;

    private:
        std::string id;
    };

    // For example :
    // $a = 6
    class VariableDeclaration : public Statement
    {
    public:
        VariableDeclaration() = default;
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
}

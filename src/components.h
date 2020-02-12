#pragma once

// All classes parsed and then compiled

#include <string>

namespace up
{
    // Casts up type to c type in string
    std::string cType(const std::string &TYPE);

    // Gives a result
    // For example :
    // 42 + 618
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
        virtual ~Statement() = default;

    public:
        // Returns the string representation of
        // the statement in C
        virtual std::string toString() const = 0;
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

    // For example :
    // $a = 6
    class VariableDeclaration : public Statement
    {
    public:
        VariableDeclaration() = default;
        VariableDeclaration(const std::string &ID, const std::string &TYPE, const Expression *EXPR);
        ~VariableDeclaration();

    public:
        virtual std::string toString() const override;

    public:
        std::string type;
        std::string id;

    private:
        // The expression which inits the variable
        const Expression *EXPR;
    };

    // Relative operation
    // For example :
    // $a += 6
    // a : ID, + : OPERATOR, 6 : EXPR 
    class VariableOperation : public Statement
    {
    public:
        VariableOperation() = default;
        VariableOperation(const std::string &ID, const Expression *EXPR, const std::string &OPERAND);
        ~VariableOperation();

    public:
        virtual std::string toString() const override;

    public:
        std::string id;

    private:
        // The expression which modifies the variable
        const Expression *EXPR;
        std::string operand;
    };
}

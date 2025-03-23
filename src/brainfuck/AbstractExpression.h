#pragma once
#ifndef RIK_BF_ABSTRACT_EXP
#define RIK_BF_ABSTRACT_EXP

namespace Rikkyu::Brainfuck {
    class IncrementExpression;
    class DecrementExpression;
    class PointerForwardExpression;
    class PointerBackwardExpression;
    class InputExpression;
    class OutputExpression;
    class LoopExpression;

    class ExpressionVisitor {
    public:
        virtual void visit(const IncrementExpression &) = 0;
        virtual void visit(const DecrementExpression &) = 0;
        virtual void visit(const PointerForwardExpression &) = 0;
        virtual void visit(const PointerBackwardExpression &) = 0;
        virtual void visit(const InputExpression &) = 0;
        virtual void visit(const OutputExpression &) = 0;
        virtual void visit(const LoopExpression &) = 0;
    };

    class Runner;
    class Expression {
    public:
        Expression() = default;
        virtual ~Expression() = default;

        virtual void               run(Runner &runner) const = 0;
        virtual void               accept(ExpressionVisitor &visitor) const = 0;
        [[nodiscard]] virtual bool repeatable() const { return false; }
        virtual void               repeat() {}
    };
}

#endif // RIK_BF_ABSTRACT_EXP
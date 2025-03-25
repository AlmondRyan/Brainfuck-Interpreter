#pragma once
#ifndef RIK_WS_STACK_EXPRESSIONS
#define RIK_WS_STACK_EXPRESSIONS

#include "../AbstractExpression.h"
#include "../interpreter.h"

namespace Rikkyu::Whitespace {
    class StackPushExpression : public Expression {
    public:
        explicit StackPushExpression(int value) : value_(value) {}

        void run(Runner &runner) const override {
            runner.memory().stackPush(value_);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

    private:
        int value_;
    };

    class StackDuplicateExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            runner.memory().stackDuplicate();
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    class StackCopyExpression : public Expression {
    public:
        explicit StackCopyExpression(int n) : n_(n) {}

        void run(Runner &runner) const override {
            runner.memory().stackCopy(n_);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

    private:
        int n_;
    };

    class StackSwapExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            runner.memory().stackSwap();
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    class StackDiscardExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            runner.memory().stackDiscard();
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    class StackSlideExpression : public Expression {
    public:
        explicit StackSlideExpression(int n) : n_(n) {}

        void run(Runner &runner) const override {
            runner.memory().stackSlide(n_);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

    private:
        int n_;
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WS_STACK_EXPRESSIONS
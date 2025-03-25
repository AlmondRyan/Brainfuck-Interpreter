#pragma once
#ifndef RIK_WS_ARITHMETIC_EXPRESSIONS
#define RIK_WS_ARITHMETIC_EXPRESSIONS

#include "../AbstractExpression.h"
#include "../interpreter.h"
#include "../../utils/ErrorHandler/ErrorHandler.h"

namespace Rikkyu::Whitespace {
    class ArithmeticAddExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            auto &memory = runner.memory();
            int   b = memory.stackPop();
            int   a = memory.stackPop();
            memory.stackPush(a + b);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    class ArithmeticSubExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            auto &memory = runner.memory();
            int   b = memory.stackPop();
            int   a = memory.stackPop();
            memory.stackPush(a - b);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    class ArithmeticMulExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            auto &memory = runner.memory();
            int   b = memory.stackPop();
            int   a = memory.stackPop();
            memory.stackPush(a * b);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    class ArithmeticDivExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            auto &memory = runner.memory();
            int   b = memory.stackPop();
            int   a = memory.stackPop();
            if (b == 0) {
                utils::ErrorHandler::getInstance().makeError("[WSE07]: Division by Zero.", 0);
            }
            memory.stackPush(a / b);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    class ArithmeticModExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            auto &memory = runner.memory();
            int   b = memory.stackPop();
            int   a = memory.stackPop();
            if (b == 0) {
                utils::ErrorHandler::getInstance().makeError("[WSE08]: Mod by Zero.", 0);
            }
            memory.stackPush(a % b);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WS_ARITHMETIC_EXPRESSIONS
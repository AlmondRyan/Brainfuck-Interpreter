#pragma once
#ifndef RIK_WS_ARITHMETIC_EXPRESSIONS
#define RIK_WS_ARITHMETIC_EXPRESSIONS

#include "../AbstractExpression.h"
#include "../Runner.h"
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

        std::string toIR() const override {
            return "ADD";
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

        std::string toIR() const override {
            return "SUB";
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

        std::string toIR() const override {
            return "MUL";
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

        std::string toIR() const override {
            return "DIV";
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

        std::string toIR() const override {
            return "MOD";
        }
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WS_ARITHMETIC_EXPRESSIONS
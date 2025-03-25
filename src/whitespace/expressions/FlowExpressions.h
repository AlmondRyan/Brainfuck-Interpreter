#pragma once
#ifndef RIK_WS_FLOW_EXPRESSIONS
#define RIK_WS_FLOW_EXPRESSIONS

#include "../AbstractExpression.h"
#include "../interpreter.h"
#include "../../utils/ErrorHandler/ErrorHandler.h"
#include "../../utils/StringBuilder/StringBuilder.h"

namespace Rikkyu::Whitespace {
    class FlowMarkExpression : public Expression {
    public:
        explicit FlowMarkExpression(std::string label, size_t position)
            : label_(std::move(label)), position_(position) {}

        void run(Runner &runner) const override {
            runner.setLabel(label_, position_);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

    private:
        std::string label_;
        size_t      position_;
    };

    class FlowCallExpression : public Expression {
    public:
        explicit FlowCallExpression(std::string label) : label_(std::move(label)) {}

        void run(Runner &runner) const override {
            runner.call(label_);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

    private:
        std::string label_;
    };

    class FlowJumpExpression : public Expression {
    public:
        explicit FlowJumpExpression(std::string label) : label_(std::move(label)) {}

        void run(Runner &runner) const override {
            runner.jump(label_);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

    private:
        std::string label_;
    };

    class FlowJumpZeroExpression : public Expression {
    public:
        explicit FlowJumpZeroExpression(std::string label) : label_(std::move(label)) {}

        void run(Runner &runner) const override {
            runner.jumpIfZero(label_);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

    private:
        std::string label_;
    };

    class FlowJumpNegativeExpression : public Expression {
    public:
        explicit FlowJumpNegativeExpression(std::string label) : label_(std::move(label)) {}

        void run(Runner &runner) const override {
            runner.jumpIfNegative(label_);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

    private:
        std::string label_;
    };

    class FlowReturnExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            runner.returnFromCall();
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    class FlowExitExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            runner.exit();
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WS_FLOW_EXPRESSIONS
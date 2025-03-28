#pragma once
#ifndef RIK_WS_IO_EXPRESSIONS
#define RIK_WS_IO_EXPRESSIONS

#include "../AbstractExpression.h"
#include "../Runner.h"
#include <iostream>

namespace Rikkyu::Whitespace {
    class IOOutputCharExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            std::cout << static_cast<char>(runner.memory().stackPop());
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

        std::string toIR() const override {
            return "OUTCHAR";
        }
    };

    class IOOutputNumExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            std::cout << runner.memory().stackPop();
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

        std::string toIR() const override {
            return "OUTNUM";
        }
    };

    class IOInputCharExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            char c;
            std::cin >> c;
            runner.memory().heapStore(runner.memory().stackPop(), static_cast<int>(c));
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

        std::string toIR() const override {
            return "INCHAR";
        }
    };

    class IOInputNumExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            int n;
            std::cin >> n;
            runner.memory().heapStore(runner.memory().stackPop(), n);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }

        std::string toIR() const override {
            return "INNUM";
        }
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WS_IO_EXPRESSIONS
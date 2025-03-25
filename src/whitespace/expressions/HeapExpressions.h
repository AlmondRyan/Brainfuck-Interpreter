#pragma once
#ifndef RIK_WS_HEAP_EXPRESSIONS
#define RIK_WS_HEAP_EXPRESSIONS

#include "../AbstractExpression.h"
#include "../interpreter.h"

namespace Rikkyu::Whitespace {
    class HeapStoreExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            auto &memory = runner.memory();
            int   value = memory.stackPop();
            int   address = memory.stackPop();
            memory.heapStore(address, value);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    class HeapRetrieveExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            auto &memory = runner.memory();
            int   address = memory.stackPop();
            memory.stackPush(memory.heapRetrieve(address));
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WS_HEAP_EXPRESSIONS
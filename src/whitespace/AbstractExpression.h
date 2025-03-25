#pragma once
#ifndef RIK_WS_ABSTRACT_EXP
#define RIK_WS_ABSTRACT_EXP

namespace Rikkyu::Whitespace {
    // 前向声明所有Whitespace指令类型
    class StackPushExpression;
    class StackDuplicateExpression;
    class StackCopyExpression;
    class StackSwapExpression;
    class StackDiscardExpression;
    class StackSlideExpression;
    class ArithmeticAddExpression;
    class ArithmeticSubExpression;
    class ArithmeticMulExpression;
    class ArithmeticDivExpression;
    class ArithmeticModExpression;
    class HeapStoreExpression;
    class HeapRetrieveExpression;
    class FlowMarkExpression;
    class FlowCallExpression;
    class FlowJumpExpression;
    class FlowJumpZeroExpression;
    class FlowJumpNegativeExpression;
    class FlowReturnExpression;
    class FlowExitExpression;
    class IOOutputCharExpression;
    class IOOutputNumExpression;
    class IOInputCharExpression;
    class IOInputNumExpression;

    // 访问者模式接口，用于遍历和处理表达式
    class ExpressionVisitor {
    public:
        virtual void visit(const StackPushExpression &) = 0;
        virtual void visit(const StackDuplicateExpression &) = 0;
        virtual void visit(const StackCopyExpression &) = 0;
        virtual void visit(const StackSwapExpression &) = 0;
        virtual void visit(const StackDiscardExpression &) = 0;
        virtual void visit(const StackSlideExpression &) = 0;
        virtual void visit(const ArithmeticAddExpression &) = 0;
        virtual void visit(const ArithmeticSubExpression &) = 0;
        virtual void visit(const ArithmeticMulExpression &) = 0;
        virtual void visit(const ArithmeticDivExpression &) = 0;
        virtual void visit(const ArithmeticModExpression &) = 0;
        virtual void visit(const HeapStoreExpression &) = 0;
        virtual void visit(const HeapRetrieveExpression &) = 0;
        virtual void visit(const FlowMarkExpression &) = 0;
        virtual void visit(const FlowCallExpression &) = 0;
        virtual void visit(const FlowJumpExpression &) = 0;
        virtual void visit(const FlowJumpZeroExpression &) = 0;
        virtual void visit(const FlowJumpNegativeExpression &) = 0;
        virtual void visit(const FlowReturnExpression &) = 0;
        virtual void visit(const FlowExitExpression &) = 0;
        virtual void visit(const IOOutputCharExpression &) = 0;
        virtual void visit(const IOOutputNumExpression &) = 0;
        virtual void visit(const IOInputCharExpression &) = 0;
        virtual void visit(const IOInputNumExpression &) = 0;
    };

    class Runner;
    // 表达式基类
    class Expression {
    public:
        Expression() = default;
        virtual ~Expression() = default;

        virtual void run(Runner &runner) const = 0;
        virtual void accept(ExpressionVisitor &visitor) const = 0;
    };
}

#endif // RIK_WS_ABSTRACT_EXP
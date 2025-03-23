#pragma once
#ifndef RIK_WHITESPACE_INTERPRETER
#define RIK_WHITESPACE_INTERPRETER

#include <array>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "../utils/ErrorHandler/ErrorHandler.h"
#include "AbstractExpression.h"
#include "defs/defs.hpp"

namespace Rikkyu::Whitespace {
    class Errors {
    public:
        void makeError(std::string message, size_t position) {
            handler.makeError(message, position);
        }

        void makeWarning(std::string message, size_t position) {
            handler.makeWarning(message, position);
        }

        void printErrors() {
            handler.printErrors();
        }
    private:
        utils::ErrorHandler handler;
    };

    class Memory {
    public:
        Memory() = default;
        ~Memory() = default;

        RIK_INLINE void stackPush(int value) {
            stack_.push(value);
        }

        RIK_INLINE int stackPop() {
            if (stack_.empty()) {
                throw std::runtime_error("Stack underflow");
            }
            int value = stack_.top();
            stack_.pop();
            return value;
        }

        [[nodiscard]] RIK_INLINE int stackPeek() const {
            if (stack_.empty()) {
                throw std::runtime_error("Stack is empty");
            }
            return stack_.top();
        }

        RIK_INLINE void stackDuplicate() {
            if (stack_.empty()) {
                throw std::runtime_error("Stack is empty");
            }
            stack_.push(stack_.top());
        }

        RIK_INLINE void stackCopy(int n) {
            if (stack_.size() <= static_cast<size_t>(n) || n < 0) {
                throw std::runtime_error("Invalid stack position");
            }

            // 创建临时栈来获取第n个元素
            std::stack<int> temp;
            for (int i = 0; i < n; ++i) {
                temp.push(stackPop());
            }

            int value = stack_.top();

            // 恢复栈
            while (!temp.empty()) {
                stack_.push(temp.top());
                temp.pop();
            }

            stack_.push(value);
        }

        RIK_INLINE void stackSwap() {
            if (stack_.size() < 2) {
                throw std::runtime_error("Not enough elements to swap");
            }

            int a = stackPop();
            int b = stackPop();

            stack_.push(a);
            stack_.push(b);
        }

        RIK_INLINE void stackDiscard() {
            if (stack_.empty()) {
                throw std::runtime_error("Stack is empty");
            }
            stack_.pop();
        }

        RIK_INLINE void stackSlide(int n) {
            if (stack_.size() < static_cast<size_t>(n) || n < 0) {
                throw std::runtime_error("Invalid slide count");
            }

            int top = stackPop();
            for (int i = 0; i < n; ++i) {
                stackPop();
            }
            stack_.push(top);
        }

        // 堆操作
        RIK_INLINE void heapStore(int address, int value) {
            heap_[address] = value;
        }

        RIK_INLINE int heapRetrieve(int address) {
            auto it = heap_.find(address);
            if (it == heap_.end()) {
                return 0; // 未初始化的地址返回0
            }
            return it->second;
        }

        RIK_INLINE bool stackEmpty() const {
            return stack_.empty();
        }

        RIK_INLINE size_t stackSize() const {
            return stack_.size();
        }

    private:
        std::stack<int>    stack_;
        std::map<int, int> heap_; // 使用map实现稀疏堆
    };

    using ExpressionPtr = std::unique_ptr<Expression>;
    using ExpressionVector = std::vector<ExpressionPtr>;

    // 运行时环境
    class Runner {
    public:
        Runner() : memory_(), callStack_() {}
        ~Runner() = default;

        inline Memory &memory() {
            return memory_;
        }

        void run(const ExpressionVector &expressions) {
            size_t pc = 0;
            while (pc < expressions.size()) {
                expressions[pc]->run(*this);
                if (jumpTo_ != static_cast<size_t>(-1)) {
                    pc = jumpTo_;
                    jumpTo_ = static_cast<size_t>(-1);
                } else {
                    ++pc;
                }
            }
            handler_.printErrors();
        }

        void reportError(const std::string &message, size_t position) {
            handler_.makeError(message, position);
        }

        void reportWarning(const std::string &message, size_t position) {
            handler_.makeWarning(message, position);
        }

        utils::ErrorHandler &getErrorHandler() {
            return handler_;
        }

        // 流程控制
        void setLabel(const std::string &label, size_t position) {
            labels_[label] = position;
        }

        void jump(const std::string &label) {
            auto it = labels_.find(label);
            if (it == labels_.end()) {
                throw std::runtime_error("Undefined label: " + label);
            }
            jumpTo_ = it->second;
        }

        void jumpIfZero(const std::string &label) {
            if (memory_.stackPop() == 0) {
                jump(label);
            }
        }

        void jumpIfNegative(const std::string &label) {
            if (memory_.stackPop() < 0) {
                jump(label);
            }
        }

        void call(const std::string &label) {
            auto it = labels_.find(label);
            if (it == labels_.end()) {
                throw std::runtime_error("Undefined label: " + label);
            }
            callStack_.push(jumpTo_ != static_cast<size_t>(-1) ? jumpTo_ : static_cast<size_t>(-1));
            jumpTo_ = it->second;
        }

        void returnFromCall() {
            if (callStack_.empty()) {
                throw std::runtime_error("Call stack underflow");
            }
            jumpTo_ = callStack_.top();
            callStack_.pop();
        }

        void exit() {
            jumpTo_ = static_cast<size_t>(-1);
            throw std::runtime_error("Program terminated");
        }

    private:
        Memory                        memory_;
        utils::ErrorHandler           handler_;
        std::map<std::string, size_t> labels_;
        std::stack<size_t>            callStack_;
        size_t                        jumpTo_ = static_cast<size_t>(-1);
    };

    // 指令实现
    // 栈操作指令
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

    // 算术指令
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
                throw std::runtime_error("Division by zero");
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
                throw std::runtime_error("Modulo by zero");
            }
            memory.stackPush(a % b);
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
    };

    // 堆访问指令
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

    // 流程控制指令
    class FlowMarkExpression : public Expression {
    public:
        explicit FlowMarkExpression(const std::string &label, size_t position)
            : label_(label), position_(position) {}

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
        explicit FlowCallExpression(const std::string &label) : label_(label) {}

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
        explicit FlowJumpExpression(const std::string &label) : label_(label) {}

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
        explicit FlowJumpZeroExpression(const std::string &label) : label_(label) {}

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
        explicit FlowJumpNegativeExpression(const std::string &label) : label_(label) {}

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

    // IO指令
    class IOOutputCharExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            std::cout << static_cast<char>(runner.memory().stackPop());
        }

        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
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
    };

    // 解析器
    class Parser {
    public:
        Parser() = default;
        ~Parser() = default;

        ExpressionVector parse(const std::vector<char> &code) {
            ExpressionVector expressions;
            size_t           pos = 0;

            while (pos < code.size()) {
                // 跳过注释（非空格、制表符和换行符的字符）
                if (code[pos] != ' ' && code[pos] != '\t' && code[pos] != '\n') {
                    ++pos;
                    continue;
                }

                try {
                    // 解析指令
                    if (code[pos] == ' ') { // 栈操作
                        ++pos;
                        if (pos >= code.size())
                            break;

                        if (code[pos] == ' ') { // 数字压栈
                            ++pos;
                            auto [value, newPos] = parseNumber(code, pos);
                            expressions.push_back(std::make_unique<StackPushExpression>(value));
                            pos = newPos;
                        } else if (code[pos] == '\t') { // 栈操作
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // 复制
                                ++pos;
                                auto [n, newPos] = parseNumber(code, pos);
                                expressions.push_back(std::make_unique<StackCopyExpression>(n));
                                pos = newPos;
                            } else if (code[pos] == '\t') { // 交换
                                ++pos;
                                expressions.push_back(std::make_unique<StackSwapExpression>());
                            } else if (code[pos] == '\n') { // 丢弃
                                ++pos;
                                expressions.push_back(std::make_unique<StackDiscardExpression>());
                            }
                        } else if (code[pos] == '\n') { // 栈操作
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // 复制栈顶
                                ++pos;
                                expressions.push_back(std::make_unique<StackDuplicateExpression>());
                            } else if (code[pos] == '\t') { // 滑动
                                ++pos;
                                auto [n, newPos] = parseNumber(code, pos);
                                expressions.push_back(std::make_unique<StackSlideExpression>(n));
                                pos = newPos;
                            }
                        }
                    } else if (code[pos] == '\t') { // 算术、堆访问和IO
                        ++pos;
                        if (pos >= code.size())
                            break;

                        if (code[pos] == ' ') { // 算术
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // 加法
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticAddExpression>());
                            } else if (code[pos] == '\t') { // 减法
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticSubExpression>());
                            } else if (code[pos] == '\n') { // 乘法
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticMulExpression>());
                            }
                        } else if (code[pos] == '\t') { // 算术和堆访问
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // 除法
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticDivExpression>());
                            } else if (code[pos] == '\t') { // 取模
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticModExpression>());
                            } else if (code[pos] == '\n') { // 堆存储
                                ++pos;
                                expressions.push_back(std::make_unique<HeapStoreExpression>());
                            }
                        } else if (code[pos] == '\n') { // 堆读取和IO
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // 堆读取
                                ++pos;
                                expressions.push_back(std::make_unique<HeapRetrieveExpression>());
                            } else if (code[pos] == '\t') { // 输出字符
                                ++pos;
                                expressions.push_back(std::make_unique<IOOutputCharExpression>());
                            } else if (code[pos] == '\n') { // 输出数字
                                ++pos;
                                expressions.push_back(std::make_unique<IOOutputNumExpression>());
                            }
                        }
                    } else if (code[pos] == '\n') { // 流程控制和IO输入
                        ++pos;
                        if (pos >= code.size())
                            break;

                        if (code[pos] == ' ') { // 流程控制
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // 标记位置
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowMarkExpression>(label, expressions.size()));
                                pos = newPos;
                            } else if (code[pos] == '\t') { // 调用子程序
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowCallExpression>(label));
                                pos = newPos;
                            } else if (code[pos] == '\n') { // 无条件跳转
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowJumpExpression>(label));
                                pos = newPos;
                            }
                        } else if (code[pos] == '\t') { // 流程控制
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // 条件跳转（零）
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowJumpZeroExpression>(label));
                                pos = newPos;
                            } else if (code[pos] == '\t') { // 条件跳转（负）
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowJumpNegativeExpression>(label));
                                pos = newPos;
                            } else if (code[pos] == '\n') { // 返回
                                ++pos;
                                expressions.push_back(std::make_unique<FlowReturnExpression>());
                            }
                        } else if (code[pos] == '\n') { // 流程控制和IO输入
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // 结束程序
                                ++pos;
                                expressions.push_back(std::make_unique<FlowExitExpression>());
                            } else if (code[pos] == '\t') { // 输入字符
                                ++pos;
                                expressions.push_back(std::make_unique<IOInputCharExpression>());
                            } else if (code[pos] == '\n') { // 输入数字
                                ++pos;
                                expressions.push_back(std::make_unique<IOInputNumExpression>());
                            }
                        }
                    }
                } catch (const std::exception &e) {
                    throw std::runtime_error("Parse error at position " + std::to_string(pos) + ": " + e.what());
                }
            }

            return expressions;
        }

    private:
        // 解析数字（二进制表示，空格=0，制表符=1，换行符结束）
        std::pair<int, size_t> parseNumber(const std::vector<char> &code, size_t pos) {
            if (pos >= code.size() || code[pos] != ' ' && code[pos] != '\t') {
                throw std::runtime_error("Expected number");
            }

            bool isNegative = false;
            if (code[pos] == '\t') {
                isNegative = true;
            }

            ++pos; // 跳过符号位

            int value = 0;
            while (pos < code.size() && code[pos] != '\n') {
                value <<= 1;
                if (code[pos] == '\t') {
                    value |= 1;
                } else if (code[pos] != ' ') {
                    throw std::runtime_error("Invalid number format");
                }
                ++pos;
            }

            if (pos < code.size() && code[pos] == '\n') {
                ++pos; // 跳过换行符
            }

            return {isNegative ? -value : value, pos};
        }

        // 解析标签（二进制表示，空格=0，制表符=1，换行符结束）
        std::pair<std::string, size_t> parseLabel(const std::vector<char> &code, size_t pos) {
            std::string label;

            while (pos < code.size() && code[pos] != '\n') {
                if (code[pos] == ' ') {
                    label += '0';
                } else if (code[pos] == '\t') {
                    label += '1';
                } else {
                    throw std::runtime_error("Invalid label format");
                }
                ++pos;
            }

            if (pos < code.size() && code[pos] == '\n') {
                ++pos; // 跳过换行符
            }

            return {label, pos};
        }
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WHITESPACE_INTERPRETER
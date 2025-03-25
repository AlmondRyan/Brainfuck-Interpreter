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
#include <utility>
#include <vector>

#include "../utils/ErrorHandler/ErrorHandler.h"
#include "../utils/StringBuilder/StringBuilder.h"
#include "AbstractExpression.h"
#include "defs/defs.hpp"
#include "expressions/StackExpressions.h"
#include "expressions/ArithmeticExpressions.h"
#include "expressions/HeapExpressions.h"
#include "expressions/FlowExpressions.h"
#include "expressions/IOExpressions.h"

namespace Rikkyu::Whitespace {
    class Memory {
    public:
        Memory() = default;
        ~Memory() = default;

        RIK_INLINE void stackPush(int value) {
            stack_.push(value);
        }

        RIK_INLINE int stackPop() {
            if (stack_.empty()) {
                utils::ErrorHandler::getInstance().makeError("[WSE01]: Stack underflow", 0);
            }
            int value = stack_.top();
            stack_.pop();
            return value;
        }

        [[nodiscard]] RIK_INLINE int stackPeek() const {
            if (stack_.empty()) {
                utils::ErrorHandler::getInstance().makeError("[WSE02]: Stack is Empty.", 0);
            }
            return stack_.top();
        }

        RIK_INLINE void stackDuplicate() {
            if (stack_.empty()) {
                utils::ErrorHandler::getInstance().makeError("[WSE02]: Stack is Empty.", 0);
            }
            stack_.push(stack_.top());
        }

        RIK_INLINE void stackCopy(int n) {
            if (stack_.size() <= static_cast<size_t>(n) || n < 0) {
                utils::ErrorHandler::getInstance().makeError("[WSE03]: Invalid access for main stack.", n);
            }

            std::stack<int> temp;
            for (int i = 0; i < n; ++i) {
                temp.push(stackPop());
            }

            int value = stack_.top();

            while (!temp.empty()) {
                stack_.push(temp.top());
                temp.pop();
            }

            stack_.push(value);
        }

        RIK_INLINE void stackSwap() {
            if (stack_.size() < 2) {
                utils::ErrorHandler::getInstance().makeError("[WSE04]: Elements in stack are not enough to do swap operation.", 0);
            }

            int a = stackPop();
            int b = stackPop();

            stack_.push(a);
            stack_.push(b);
        }

        RIK_INLINE void stackDiscard() {
            if (stack_.empty()) {
                utils::ErrorHandler::getInstance().makeError("[WSE02]: Stack is Empty.", 0);
            }
            stack_.pop();
        }

        RIK_INLINE void stackSlide(int n) {
            if (stack_.size() < static_cast<size_t>(n) || n < 0) {
                utils::ErrorHandler::getInstance().makeError("[WSE02]: Stack slide count is invalid.", n);
            }

            int top = stackPop();
            for (int i = 0; i < n; ++i) {
                stackPop();
            }
            stack_.push(top);
        }

        RIK_INLINE void heapStore(int address, int value) {
            heap_[address] = value;
        }

        RIK_INLINE int heapRetrieve(int address) {
            auto it = heap_.find(address);
            if (it == heap_.end()) {
                return 0;
            }
            return it->second;
        }

        [[nodiscard]] RIK_INLINE bool stackEmpty() const {
            return stack_.empty();
        }

        [[nodiscard]] RIK_INLINE size_t stackSize() const {
            return stack_.size();
        }

    private:
        std::stack<int>    stack_;
        std::map<int, int> heap_;
    };

    using ExpressionPtr = std::unique_ptr<Expression>;
    using ExpressionVector = std::vector<ExpressionPtr>;

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
        }

        void reportError(const std::string &message, size_t position) {
            utils::ErrorHandler::getInstance().makeError(message, position);
        }

        void reportWarning(const std::string &message, size_t position) {
            utils::ErrorHandler::getInstance().makeWarning(message, position);
        }

        void setLabel(const std::string &label, size_t position) {
            labels_[label] = position;
        }

        void jump(const std::string &label) {
            auto it = labels_.find(label);
            if (it == labels_.end()) {
                utils::ErrorHandler::getInstance().makeError(utils::StringBuilder::concatenate("[WSE05]: Undefined Label: ", label), 0);
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
                utils::ErrorHandler::getInstance().makeError(utils::StringBuilder::concatenate("[WSE05]: Undefined Label: ", label),
                                                             0);
            }
            callStack_.push(jumpTo_ != static_cast<size_t>(-1) ? jumpTo_ : static_cast<size_t>(-1));
            jumpTo_ = it->second;
        }

        void returnFromCall() {
            if (callStack_.empty()) {
                utils::ErrorHandler::getInstance().makeError("[WSE07]: Call stack overflow.", 0);
            }
            jumpTo_ = callStack_.top();
            callStack_.pop();
        }

        void exit() {
            jumpTo_ = static_cast<size_t>(-1);
            utils::ErrorHandler::getInstance().makeError("[WSE06]: Program unexpected terminal.", 0);
        }

    private:
        Memory                        memory_;
        std::map<std::string, size_t> labels_;
        std::stack<size_t>            callStack_;
        size_t                        jumpTo_ = static_cast<size_t>(-1);
    };





    class Parser {
    public:
        Parser() = default;
        ~Parser() = default;

        ExpressionVector parse(const std::vector<char> &code) {
            ExpressionVector expressions;
            size_t           pos = 0;

            while (pos < code.size()) {
                if (code[pos] != ' ' && code[pos] != '\t' && code[pos] != '\n') {
                    ++pos;
                    continue;
                }

                try {
                    if (code[pos] == ' ') {
                        ++pos;
                        if (pos >= code.size())
                            break;

                        if (code[pos] == ' ') { // Numbers push in stack
                            ++pos;
                            auto [value, newPos] = parseNumber(code, pos);
                            expressions.push_back(std::make_unique<StackPushExpression>(value));
                            pos = newPos;
                        } else if (code[pos] == '\t') {
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // Copy
                                ++pos;
                                auto [n, newPos] = parseNumber(code, pos);
                                expressions.push_back(std::make_unique<StackCopyExpression>(n));
                                pos = newPos;
                            } else if (code[pos] == '\t') { // Swap
                                ++pos;
                                expressions.push_back(std::make_unique<StackSwapExpression>());
                            } else if (code[pos] == '\n') { // Discarding
                                ++pos;
                                expressions.push_back(std::make_unique<StackDiscardExpression>());
                            }
                        } else if (code[pos] == '\n') {
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // Copy the top element in the stack
                                ++pos;
                                expressions.push_back(std::make_unique<StackDuplicateExpression>());
                            } else if (code[pos] == '\t') { // Slide the stack
                                ++pos;
                                auto [n, newPos] = parseNumber(code, pos);
                                expressions.push_back(std::make_unique<StackSlideExpression>(n));
                                pos = newPos;
                            }
                        }
                    } else if (code[pos] == '\t') {
                        ++pos;
                        if (pos >= code.size())
                            break;

                        if (code[pos] == ' ') { // Arithmetic
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // Add
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticAddExpression>());
                            } else if (code[pos] == '\t') { // Subtraction
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticSubExpression>());
                            } else if (code[pos] == '\n') { // Multiplication
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticMulExpression>());
                            }
                        } else if (code[pos] == '\t') {
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // Division
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticDivExpression>());
                            } else if (code[pos] == '\t') { // Modulus
                                ++pos;
                                expressions.push_back(std::make_unique<ArithmeticModExpression>());
                            } else if (code[pos] == '\n') { // Heap Store
                                ++pos;
                                expressions.push_back(std::make_unique<HeapStoreExpression>());
                            }
                        } else if (code[pos] == '\n') { // Heap Reading and IO
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') { // Heap Read
                                ++pos;
                                expressions.push_back(std::make_unique<HeapRetrieveExpression>());
                            } else if (code[pos] == '\t') { // Output Character
                                ++pos;
                                expressions.push_back(std::make_unique<IOOutputCharExpression>());
                            } else if (code[pos] == '\n') { // Output Number
                                ++pos;
                                expressions.push_back(std::make_unique<IOOutputNumExpression>());
                            }
                        }
                    } else if (code[pos] == '\n') {
                        ++pos;
                        if (pos >= code.size())
                            break;

                        if (code[pos] == ' ') {
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') {
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowMarkExpression>(label, expressions.size()));
                                pos = newPos;
                            } else if (code[pos] == '\t') {
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowCallExpression>(label));
                                pos = newPos;
                            } else if (code[pos] == '\n') {
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowJumpExpression>(label));
                                pos = newPos;
                            }
                        } else if (code[pos] == '\t') {
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') {
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowJumpZeroExpression>(label));
                                pos = newPos;
                            } else if (code[pos] == '\t') {
                                ++pos;
                                auto [label, newPos] = parseLabel(code, pos);
                                expressions.push_back(std::make_unique<FlowJumpNegativeExpression>(label));
                                pos = newPos;
                            } else if (code[pos] == '\n') {
                                ++pos;
                                expressions.push_back(std::make_unique<FlowReturnExpression>());
                            }
                        } else if (code[pos] == '\n') {
                            ++pos;
                            if (pos >= code.size())
                                break;

                            if (code[pos] == ' ') {
                                ++pos;
                                expressions.push_back(std::make_unique<FlowExitExpression>());
                            } else if (code[pos] == '\t') {
                                ++pos;
                                expressions.push_back(std::make_unique<IOInputCharExpression>());
                            } else if (code[pos] == '\n') {
                                ++pos;
                                expressions.push_back(std::make_unique<IOInputNumExpression>());
                            }
                        }
                    }
                } catch (const std::exception &e) {
                    utils::ErrorHandler::getInstance().makeError(
                        utils::StringBuilder::concatenate("[WSE09]: Parse error at: ", std::to_string(pos), ": ", e.what()),
                        0);
                }
            }

            return expressions;
        }

    private:
        std::pair<int, size_t> parseNumber(const std::vector<char> &code, size_t pos) {
            if (pos >= code.size() || code[pos] != ' ' && code[pos] != '\t') {
                utils::ErrorHandler::getInstance().makeError("[WSE10]: Expected number", 0);
            }

            bool isNegative = false;
            if (code[pos] == '\t') {
                isNegative = true;
            }

            ++pos;

            int value = 0;
            while (pos < code.size() && code[pos] != '\n') {
                value <<= 1;
                if (code[pos] == '\t') {
                    value |= 1;
                } else if (code[pos] != ' ') {
                    utils::ErrorHandler::getInstance().makeError("[WSE11]: Invalid number format", 0);
                }
                ++pos;
            }

            if (pos < code.size() && code[pos] == '\n') {
                ++pos;
            }

            return {isNegative ? -value : value, pos};
        }

        std::pair<std::string, size_t> parseLabel(const std::vector<char> &code, size_t pos) {
            std::string label;

            while (pos < code.size() && code[pos] != '\n') {
                if (code[pos] == ' ') {
                    label += '0';
                } else if (code[pos] == '\t') {
                    label += '1';
                } else {
                    utils::ErrorHandler::getInstance().makeError("[WSE12]: Invalid label format", 0);
                }
                ++pos;
            }

            if (pos < code.size() && code[pos] == '\n') {
                ++pos;
            }

            return {label, pos};
        }
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WHITESPACE_INTERPRETER
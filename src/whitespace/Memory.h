#pragma once
#ifndef RIK_WHITESPACE_MEMORY
#define RIK_WHITESPACE_MEMORY

#include <map>
#include <stack>
#include "../utils/ErrorHandler/ErrorHandler.h"

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
                utils::ErrorHandler::getInstance().makeError("[WSE01]: Stack underflow - 无法从空栈中弹出元素", 0);
            }
            int value = stack_.top();
            stack_.pop();
            return value;
        }

        [[nodiscard]] RIK_INLINE int stackPeek() const {
            if (stack_.empty()) {
                utils::ErrorHandler::getInstance().makeError("[WSE02]: Stack is Empty - 无法查看空栈的顶部元素", 0);
            }
            return stack_.top();
        }

        RIK_INLINE void stackDuplicate() {
            if (stack_.empty()) {
                utils::ErrorHandler::getInstance().makeError("[WSE02]: Stack is Empty - 无法复制空栈的顶部元素", 0);
            }
            stack_.push(stack_.top());
        }

        RIK_INLINE void stackCopy(int n) {
            if (stack_.size() <= static_cast<size_t>(n) || n < 0) {
                utils::ErrorHandler::getInstance().makeError(utils::StringBuilder::concatenate("[WSE03]: Invalid access for main stack - 无效的栈访问索引: ", std::to_string(n)), n);
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
                utils::ErrorHandler::getInstance().makeError("[WSE04]: Elements in stack are not enough to do swap operation - 栈中元素不足，无法执行交换操作", 0);
            }

            int a = stackPop();
            int b = stackPop();

            stack_.push(a);
            stack_.push(b);
        }

        RIK_INLINE void stackDiscard() {
            if (stack_.empty()) {
                utils::ErrorHandler::getInstance().makeError("[WSE02]: Stack is Empty - 无法丢弃空栈的元素", 0);
            }
            stack_.pop();
        }

        RIK_INLINE void stackSlide(int n) {
            if (stack_.size() < static_cast<size_t>(n) || n < 0) {
                utils::ErrorHandler::getInstance().makeError(utils::StringBuilder::concatenate("[WSE02]: Stack slide count is invalid - 无效的栈滑动数量: ", std::to_string(n)), n);
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
} // namespace Rikkyu::Whitespace

#endif // RIK_WHITESPACE_MEMORY
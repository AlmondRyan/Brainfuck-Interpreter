#include <array>
#include <exception>
#include <memory>
#include <vector>

#include "../utils/ErrorHandler/ErrorHandler.h"
#include "AbstractExpression.h"
#include "defs/defs.hpp"

namespace Rikkyu::Brainfuck {
    template <typename Tp = unsigned int>
    class Memory {
    public:
        Memory() : memory_(), ptr_(memory_.begin()) {}
        ~Memory() = default;

        RIK_INLINE void memory_byteIncrease(Tp offset) {
            *this->ptr_ += offset;
        }

        RIK_INLINE void memory_byteDecrease(Tp offset) {
            *this->ptr_ -= offset;
        }

        RIK_INLINE bool memory_pointerShiftForward(ssize_t offset) {
            if (ptr_ + offset >= memory_.end()) {
                return false;
            }
            this->ptr_ += offset;
            return true;
        }

        RIK_INLINE bool memory_pointerShiftBackward(ssize_t offset) {
            if (ptr_ - offset < memory_.begin()) {
                return false;
            }
            this->ptr_ -= offset;
            return true;
        }

        [[nodiscard]] RIK_INLINE Tp memory_pointerByteReadData() const {
            return *this->ptr_;
        }

        RIK_INLINE void memory_pointerByteWriteData(Tp c) {
            *this->ptr_ = c;
        }

    private:
        std::array<Tp, 30000>                memory_;
        typename decltype(memory_)::iterator ptr_;
    };

    using ExpressionPtr = std::unique_ptr<Expression>;
    using ExpressionVector = std::vector<ExpressionPtr>;

    class Runner {
    public:
        Runner() : memory_() {}
        ~Runner() = default;

        inline Memory<> &memory() {
            return memory_;
        };

        void run(const ExpressionVector &expressions) {
            for (const auto &expression : expressions) {
                expression->run(*this);
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

    private:
        Memory<>            memory_;
        utils::ErrorHandler handler_;
    };

    class IncrementExpression : public Expression {
    public:
        explicit IncrementExpression(ssize_t offset) : Expression(), offset_(offset) {}
        void run(Runner &runner) const override {
            runner.memory().memory_byteIncrease(offset_);
        }
        void accept(ExpressionVisitor &visitor) const override {
            visitor.visit(*this);
        }
        [[nodiscard]] bool repeatable() const override {
            return true;
        }
        void repeat() override {
            ++offset_;
        }
        [[maybe_unused]] [[nodiscard]] ssize_t offset() const {
            return offset_;
        }

    private:
        ssize_t offset_;
    };

    class DecrementExpression : public Expression {
    public:
        explicit DecrementExpression(ssize_t offset) : Expression(), offset_(offset) {}
        void run(Runner &runner) const override {
            runner.memory().memory_byteDecrease(offset_);
        }
        virtual void accept(ExpressionVisitor &visitor) const {
            visitor.visit(*this);
        }
        virtual bool repeatable() const {
            return true;
        }
        virtual void repeat() {
            ++offset_;
        }
        ssize_t offset() const {
            return offset_;
        }

    private:
        ssize_t offset_;
    };

    class PointerForwardExpression : public Expression {
    public:
        PointerForwardExpression(ssize_t offset) : Expression(), offset_(offset) {}
        void run(Runner &runner) const override {
            if (!runner.memory().memory_pointerShiftForward(offset_)) {
                runner.reportError("[BFE01]: Memory pointer forward out of bounds", 0);
            }
        }
        virtual void accept(ExpressionVisitor &visitor) const {
            visitor.visit(*this);
        }
        virtual bool repeatable() const {
            return true;
        }
        virtual void repeat() {
            ++offset_;
        }
        ssize_t offset() const {
            return offset_;
        }

    private:
        ssize_t offset_;
    };

    class PointerBackwardExpression : public Expression {
    public:
        explicit PointerBackwardExpression(ssize_t offset) : Expression(), offset_(offset) {}
        void run(Runner &runner) const override {
            if (!runner.memory().memory_pointerShiftBackward(offset_)) {
                runner.reportError("[BFE02]: Memory pointer backward out of bounds", 0);
            }
        }
        virtual void accept(ExpressionVisitor &visitor) const {
            visitor.visit(*this);
        }
        virtual bool repeatable() const {
            return true;
        }
        virtual void repeat() {
            ++offset_;
        }
        ssize_t offset() const {
            return offset_;
        }

    private:
        ssize_t offset_;
    };

    class InputExpression : public Expression {
    public:
        void run(Runner &runner) const override {
            int ch = getchar();
            if (ch == EOF) {
                runner.reportWarning("[BFW01]: Input stream reached EOF.", 0);
                return;
            }
            runner.memory().memory_pointerByteWriteData(static_cast<unsigned int>(ch));
        }
        virtual void accept(ExpressionVisitor &visitor) const {
            visitor.visit(*this);
        }

        virtual bool repeatable() const {
            return false;
        }

        virtual void repeat() {}
    };

    class OutputExpression : public Expression {
    public:
        virtual void run(Runner &runner) const {
            putchar(runner.memory().memory_pointerByteReadData());
            fflush(stdout);
        }

        virtual void accept(ExpressionVisitor &visitor) const {
            visitor.visit(*this);
        }

        virtual bool repeatable() const {
            return false;
        }

        virtual void repeat() {}
    };

    class LoopExpression : public Expression {
    public:
        explicit LoopExpression(ExpressionVector &&children)
            : Expression(),
              children_(std::move(children)) {}

        LoopExpression(const LoopExpression &) = delete;

        [[nodiscard]] const ExpressionVector &children() const {
            return children_;
        }

        void run(Runner &runner) const override {
            while (runner.memory().memory_pointerByteReadData() > 0) {
                runner.run(children_);
            }
        }

        virtual void accept(ExpressionVisitor &visitor) const {
            visitor.visit(*this);
        }

        virtual bool repeatable() const {
            return false;
        }

        void repeat() override {}

    private:
        ExpressionVector children_;
    };

    using TokenVector = std::vector<char>;

    class Parser {
    public:
        Parser() = default;
        ~Parser() = default;

        ExpressionVector parse(TokenVector &);

    private:
        utils::ErrorHandler handler;
    };

    ExpressionVector Parser::parse(TokenVector &tokens) {
        using ExpressionVectorPtr = std::unique_ptr<ExpressionVector>;

        std::vector<ExpressionVectorPtr> stack;
        ExpressionVectorPtr              expressions(new ExpressionVector());
        size_t                           position = 0;

        for (auto token : tokens) {
            ExpressionPtr next;
            position++;

            switch (token) {
            case '+':
                next = ExpressionPtr(new IncrementExpression(1));
                break;
            case '-':
                next = ExpressionPtr(new DecrementExpression(1));
                break;
            case '>':
                next = ExpressionPtr(new PointerForwardExpression(1));
                break;
            case '<':
                next = ExpressionPtr(new PointerBackwardExpression(1));
                break;
            case ',':
                next = ExpressionPtr(new InputExpression());
                break;
            case '.':
                next = ExpressionPtr(new OutputExpression());
                break;
            case '[':
                stack.push_back(std::move(expressions));
                expressions = std::make_unique<ExpressionVector>();
                break;
            case ']':
                if (stack.empty()) {
                    handler.makeError("[BFE03]: 未匹配的 ']'", position);
                    return {};
                }
                next = ExpressionPtr(new LoopExpression(std::move(*expressions)));
                expressions = std::move(stack.back());
                stack.pop_back();
                break;
            default:
                // 忽略其他字符
                continue;
            }

            if (!next) {
                continue;
            } else if (expressions->empty() ||
                       typeid(*expressions->back()) != typeid(*next) ||
                       !expressions->back()->repeatable()) {
                expressions->push_back(std::move(next));
            } else {
                expressions->back()->repeat();
            }
        }

        // 检查是否有未匹配的 '['
        if (!stack.empty()) {
            handler.makeError("[BFE03]: 未匹配的 '['", position);
            return {};
        }

        handler.printErrors();
        if (handler.hasErrors()) {
            return {};
        } else {
            return std::move(*expressions);
        }
    }
} // namespace Rikkyu::Brainfuck
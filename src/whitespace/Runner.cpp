#include "Runner.h"
#include "Memory.h"
#include "AbstractExpression.h"

namespace Rikkyu::Whitespace {
    Runner::Runner() : memory_(new Memory()), callStack_() {}

    Memory &Runner::memory() {
        return *memory_;
    }

    void Runner::run(const ExpressionVector &expressions, bool showIR) {
        size_t pc = 0;
        while (pc < expressions.size()) {
            if (showIR) {
                std::cout << "[" << pc << "] " << expressions[pc]->toIR() << std::endl;
            }
            expressions[pc]->run(*this);
            if (jumpTo_ != static_cast<size_t>(-1)) {
                pc = jumpTo_;
                jumpTo_ = static_cast<size_t>(-1);
            } else { 
                ++pc;
            }
        }
    }

    void Runner::reportError(const std::string &message, size_t position) {
        utils::ErrorHandler::getInstance().makeError(message, position);
    }

    void Runner::reportWarning(const std::string &message, size_t position) {
        utils::ErrorHandler::getInstance().makeWarning(message, position);
    }

    void Runner::setLabel(const std::string &label, size_t position) {
        labels_[label] = position;
    }

    void Runner::jump(const std::string &label) {
        auto it = labels_.find(label);
        if (it == labels_.end()) {
            utils::ErrorHandler::getInstance().makeError(utils::StringBuilder::concatenate("[WSE05]: Undefined Label: ", label), 0);
        }
        jumpTo_ = it->second;
    }

    void Runner::jumpIfZero(const std::string &label) {
        if (memory_->stackPop() == 0) {
            jump(label);
        }
    }

    void Runner::jumpIfNegative(const std::string &label) {
        if (memory_->stackPop() < 0) {
            jump(label);
        }
    }

    void Runner::call(const std::string &label) {
        auto it = labels_.find(label);
        if (it == labels_.end()) {
            utils::ErrorHandler::getInstance().makeError(utils::StringBuilder::concatenate("[WSE05]: Undefined Label: ", label), 0);
        }
        callStack_.push(jumpTo_ != static_cast<size_t>(-1) ? jumpTo_ : static_cast<size_t>(-1));
        jumpTo_ = it->second;
    }

    void Runner::returnFromCall() {
        if (callStack_.empty()) {
            utils::ErrorHandler::getInstance().makeError("[WSE07]: Call stack overflow.", 0);
        }
        jumpTo_ = callStack_.top();
        callStack_.pop();
    }

    void Runner::exit() {
        jumpTo_ = static_cast<size_t>(-1);
        utils::ErrorHandler::getInstance().makeError("[WSE06]: Program unexpected terminal.", 0);
    }
} // namespace Rikkyu::Whitespace
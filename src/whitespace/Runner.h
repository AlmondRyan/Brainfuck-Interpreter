#pragma once
#ifndef RIK_WHITESPACE_RUNNER
#define RIK_WHITESPACE_RUNNER

#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <iostream>

#include "../utils/ErrorHandler/ErrorHandler.h"
#include "../utils/StringBuilder/StringBuilder.h"
#include "AbstractExpression.h"

namespace Rikkyu::Whitespace {
    class Memory;
    class Expression;
    
    using ExpressionPtr = std::unique_ptr<Expression>;
    using ExpressionVector = std::vector<ExpressionPtr>;
    
    class Runner {
    public:
        Runner();
        ~Runner() = default;
        
        Memory &memory();
        
        void run(const ExpressionVector &expressions, bool showIR = false);
        
        void reportError(const std::string &message, size_t position);
        
        void reportWarning(const std::string &message, size_t position);
        
        void setLabel(const std::string &label, size_t position);
        
        void jump(const std::string &label);
        
        void jumpIfZero(const std::string &label);
        
        void jumpIfNegative(const std::string &label);
        
        void call(const std::string &label);
        
        void returnFromCall();
        
        void exit();
        
    private:
        Memory *memory_;
        std::map<std::string, size_t> labels_;
        std::stack<size_t> callStack_;
        size_t jumpTo_ = static_cast<size_t>(-1);
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WHITESPACE_RUNNER
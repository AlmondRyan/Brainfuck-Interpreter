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
#include "Memory.h"
#include "Runner.h"
#include "expressions/StackExpressions.h"
#include "expressions/ArithmeticExpressions.h"
#include "expressions/HeapExpressions.h"
#include "expressions/FlowExpressions.h"
#include "expressions/IOExpressions.h"

namespace Rikkyu::Whitespace {
    class Parser {
    public:
        Parser() = default;
        ~Parser() = default;
        
        // 计算给定位置的行号和列号
        std::pair<size_t, size_t> calculateLineCol(const std::vector<char> &code, size_t pos) {
            size_t line = 1;
            size_t col = 1;
            
            for (size_t i = 0; i < pos && i < code.size(); ++i) {
                if (code[i] == '\n') {
                    line++;
                    col = 1;
                } else {
                    col++;
                }
            }
            
            return {line, col};
        }

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

                        if (code[pos] == ' ') { // Stack manipulation
                            ++pos;
                            if (pos >= code.size()) break;
                            
                            if (code[pos] == ' ') { // Push number
                                ++pos;
                                auto [value, newPos] = parseNumber(code, pos);
                                expressions.push_back(std::make_unique<StackPushExpression>(value));
                                pos = newPos;
                            }
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
                    auto [line, col] = calculateLineCol(code, pos);
                    utils::ErrorHandler::getInstance().makeError(
                        utils::StringBuilder::concatenate("[WSE09]: Parse error at line ", std::to_string(line), ", column ", std::to_string(col), ": ", e.what()),
                        pos);
                }
            }

            return expressions;
        }

    private:
        std::pair<int, size_t> parseNumber(const std::vector<char> &code, size_t pos) {
            if (pos >= code.size()) {
                auto [line, col] = calculateLineCol(code, pos);
                utils::ErrorHandler::getInstance().makeError(
                    utils::StringBuilder::concatenate("[WSE10]: Expected number at line ", std::to_string(line), ", column ", std::to_string(col)),
                    pos);
            }

            bool isNegative = false;
            if (code[pos] == '\t') {
                isNegative = true;
            } else if (code[pos] != ' ') {
                auto [line, col] = calculateLineCol(code, pos);
                utils::ErrorHandler::getInstance().makeError(
                    utils::StringBuilder::concatenate("[WSE11]: Invalid number sign at line ", std::to_string(line), ", column ", std::to_string(col)),
                    pos);
            }

            ++pos;
            int value = 0;
            while (pos < code.size() && code[pos] != '\n') {
                if (code[pos] == ' ' || code[pos] == '\t') {
                    value <<= 1;
                    if (code[pos] == '\t') {
                        value |= 1;
                    }
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
                if (code[pos] == ' ' || code[pos] == '\t') {
                    label += (code[pos] == ' ' ? '0' : '1');
                }
                ++pos;
            }

            if (pos < code.size() && code[pos] == '\n') {
                ++pos;
            }

            if (label.empty()) {
                auto [line, col] = calculateLineCol(code, pos);
                utils::ErrorHandler::getInstance().makeError(
                    utils::StringBuilder::concatenate("[WSE12]: Empty label at line ", std::to_string(line), ", column ", std::to_string(col)),
                    pos);
            }

            return {label, pos};
        }
    };
} // namespace Rikkyu::Whitespace

#endif // RIK_WHITESPACE_INTERPRETER
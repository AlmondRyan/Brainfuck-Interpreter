#pragma once
#ifndef RIK_ERRORHANDLER_H
#define RIK_ERRORHANDLER_H

#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include "defs/defs.hpp"

namespace Rikkyu::utils {
    enum class ErrorType {
        ET_Notice,
        ET_CriticalError,
        ET_Warning,
    };

    struct ErrorObj {
        ErrorType   type;
        size_t      pos;
        std::string text;

        explicit ErrorObj(ErrorType _t, size_t _p, std::string _te)
            : type(_t), pos(_p), text(std::move(_te)) {}
    };

    class ErrorHandler {
    public:
        void makeWarning(const std::string &warningText, size_t pos);
        void makeError(const std::string &errorText, size_t pos);
        void makeNotice(const std::string &noteText, size_t pos);

        void printErrors();
        bool hasErrors() {
            return !errors.empty();
        }
    private:
        std::vector<ErrorObj> errors;
    };
} // namespace Rikkyu::utils

#endif // RIK_ERRORHANDLER_H
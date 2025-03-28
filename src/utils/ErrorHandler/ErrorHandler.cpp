#include "ErrorHandler.h"
#include "utils/ConsoleTextManager/ConsoleTextManager.h"

namespace Rikkyu::utils {
    // 单例实例获取方法
    ErrorHandler& ErrorHandler::getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    void ErrorHandler::makeWarning(const std::string &warningText, size_t pos) {
        errors.emplace_back(ErrorType::ET_Warning, pos, warningText);
    }

    void ErrorHandler::makeError(const std::string &errorText, size_t pos) {
        errors.emplace_back(ErrorType::ET_CriticalError, pos, errorText);
    }

    void ErrorHandler::makeNotice(const std::string &noteText, size_t pos) {
        errors.emplace_back(ErrorType::ET_Notice, pos, noteText);
    }

    void ErrorHandler::printErrors() {
        if (errors.empty()) {
            return;
        }

        for (auto &i : errors) {
            if (i.type == ErrorType::ET_CriticalError) {
                std::cout << Rikkyu::utils::Colors::CTM_Red << "[Error]" <<
                Rikkyu::utils::Colors::CTM_Default << ": " << i.text << std::endl;
            } else if (i.type == ErrorType::ET_Warning) {
                std::cout << Rikkyu::utils::Colors::CTM_Yellow << "[Warning]" <<
                Rikkyu::utils::Colors::CTM_Default << ": " << i.text << std::endl;
            } else if (i.type == ErrorType::ET_Notice) {
                std::cout << Rikkyu::utils::Colors::CTM_Blue << "[Note]" <<
                    Rikkyu::utils::Colors::CTM_Default << ": " << i.text << std::endl;
            }
        }
    }

    void ErrorHandler::clearErrors() {
        errors.clear();
    }
}
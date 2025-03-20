#include <concepts>
#include <iostream>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <stack>

#ifdef _WIN32
#include <windows.h>
#endif

namespace BrainfuckInterpreter {
    /**
     * @brief ErrorType enumeration.
     *
     * A helper for ErrorHandler class.
     */
    enum ErrorType {
        ET_CriticalError,
        ET_Warning
    };

    /**
     * @brief ErrorObj structure.
     *
     * A single object for ErrorHandler. Each error in ErrorHandler is a Error Object. It contains
     * the type of Error (Critical Error or Warning), and position that error found, and the
     * descriptor of the error.
     *
     * @sa ErrorType
     */
    struct ErrorObj {
        ErrorType type;
        size_t pos;
        std::string text;

        ErrorObj(ErrorType _t, size_t _p, std::string _te) : type(_t), pos(_p), text(std::move(_te)) {}
    };

    /**
     * @brief ErrorHandler class.
     */
    class ErrorHandler {
    public:
        /**
         * @brief Make a Warning Object.
         *
         * @param warningText The descriptor of the warning.
         * @param pos The position that warning found.
         */
        void makeWarning(const std::string &warningText, size_t pos) {
            errors.emplace_back(ErrorType::ET_Warning, pos, warningText);
        }

        /**
         * @brief Make a Error Object.
         *
         * @param errorText The descriptor of the error.
         * @param pos The position that error found.
         */
        void makeError(const std::string &errorText, size_t pos) {
            errors.emplace_back(ErrorType::ET_CriticalError, pos, errorText);
        }

        /**
         * @brief Print the errors to stdout.
         *
         * This function contains the logic of printing the errors in `errors` vector.
         * This function is a multi-platform function, it will check the platform
         * is Windows or macOS or Linux, automatically using the method for coloring
         * the type of Error. Such as Error(Red) and Warning(Yellow).
         */
        void printErrors() {
            if (errors.empty()) {
                return;
            }

            for (auto &i: errors) {
                if (i.type == ErrorType::ET_CriticalError) {
#ifdef _WIN32
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    std::cout << "[Error]";
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                    std::cout << ": At " << i.pos << " , " << i.text << std::endl;
#else
                    std::cout << "\033[31m[Error]\033[0m: At " << i.pos << " , " << i.text << std::endl;
#endif
                } else if (i.type == ErrorType::ET_Warning) {
#ifdef _WIN32
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
                    std::cout << "[Warning]";
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                    std::cout << ": At " << i.pos << " , " << i.text << std::endl;
#else
                    std::cout << "\033[33m[Warning]\033[0m: At " << i.pos << " , " << i.text << std::endl;
#endif
                }
            }
        }

    private:
        std::vector<ErrorObj> errors;
    };

    /**
     * @brief The Interpreter itself.
     *
     * Now comes the main part, the Interpreter itself!
     * But the first version, it only support C++20 or above, but trust me, I will add a
     * logic to make C++11 and above compatible.
     */
    class BrainfuckInterpreter {
    public:
        /**
         * @brief Unified interpreting entry.
         * 
         * This function automatically selects the appropriate implementation
         * based on the C++ standard supported by the compiler.
         * 
         * @tparam Tp The code type. It need to be vector<uint8_t> or string.
         * @param code The code itself.
         * @return The result after interpreting.
         */
        template<typename Tp>
        std::string interpret(const Tp &code) {
#if __cplusplus >= 202002L
            return interpret_cpp20(code);
#elif __cplusplus >= 201703L
            return interpret_cpp17(code);
#elif __cplusplus >= 201402L
            return interpret_cpp14(code);
#else
            return interpret_cpp11(code);
#endif
        }

        /**
         * @brief Write the errors to console.
         */
        void writeErrors() {
            handler.printErrors();
        }

    public:
        ErrorHandler handler;
    private:
#if __cplusplus >= 202002L
        /**
         * @brief Interpreting entry. (For C++20)
         *
         * @tparam Tp The code type. It need to be vector<uint8_t> or string.
         * @param code The code itself.
         * @return The result after interpreting.
         */
        template<typename Tp>
        requires (std::same_as<Tp, std::vector<uint8_t>> || std::same_as<Tp, std::string>)
        std::string interpret_cpp20(const Tp &code) {
            std::string finalResult;
            if constexpr (std::same_as<Tp, std::vector<uint8_t>>) {
                finalResult = interpret_internal(code);
                writeErrors();
            } else if constexpr (std::same_as<Tp, std::string>) {
                finalResult = interpret_internal(trans_string_to_vec(code));
                writeErrors();
            }

            return finalResult;
        }
#endif

#if __cplusplus >= 201703L && __cplusplus < 202002L
        /**
         * @brief Interpreting entry. (For C++17)
         * 
         * @tparam Tp The code type. It need to be vector<uint8_t> or string.
         * @param code The code itself.
         * @return The result after interpreting.
         */
        template<typename Tp>
        std::string interpret_cpp17(const Tp &code, 
            typename std::enable_if<
                std::is_same<Tp, std::vector<uint8_t>>::value || 
                std::is_same<Tp, std::string>::value
            >::type* = nullptr) {
            
            std::string finalResult;
            if (std::is_same<Tp, std::vector<uint8_t>>::value) {
                finalResult = interpret_internal(code);
            } else {
                finalResult = interpret_internal(trans_string_to_vec(code));
            }
            writeErrors();
            return finalResult;
        }
#endif

#if __cplusplus >= 201402L && __cplusplus < 201703L
        /**
         * @brief Interpreting entry. (For C++14)
         * 
         * @tparam Tp The code type. It need to be vector<uint8_t> or string.
         * @param code The code itself.
         * @return The result after interpreting.
         */
        template<typename Tp>
        typename std::enable_if<
            std::is_same<Tp, std::vector<uint8_t>>::value || 
            std::is_same<Tp, std::string>::value,
            std::string
        >::type interpret_cpp14(const Tp &code) {
            std::string finalResult;
            if (std::is_same<Tp, std::vector<uint8_t>>::value) {
                finalResult = interpret_internal(code);
            } else {
                finalResult = interpret_internal(trans_string_to_vec(code));
            }
            writeErrors();
            return finalResult;
        }
#endif

        /**
         * @brief Interpreting entry. (For C++11)
         * 
         * @param code The code itself (vector<uint8_t>).
         * @return The result after interpreting.
         */
        std::string interpret_cpp11(const std::vector<uint8_t> &code) {
            std::string finalResult = interpret_internal(code);
            writeErrors();
            return finalResult;
        }

        /**
         * @brief Interpreting entry. (For C++11)
         * 
         * @param code The code itself (string).
         * @return The result after interpreting.
         */
        std::string interpret_cpp11(const std::string &code) {
            std::string finalResult = interpret_internal(trans_string_to_vec(code));
            writeErrors();
            return finalResult;
        }

        /**
         * @brief The internal implementation of interpreter.
         *
         * @param m_code The code itself.
         * @return The result after interpreting.
         */
        std::string interpret_internal(std::vector<uint8_t> m_code) {
            std::string res;
            auto jumpTable = buildJumpTable(m_code);
            if (jumpTable.empty()) {
                return {};
            }

            for (size_t i = 0; i < m_code.size(); ++i) {
                char cmd = m_code[i];
                switch (cmd) {
                    case '>': {
                        if (currentIndex + 1 >= tape.size()) {
                            handler.makeError("Pointer out of bounds.", currentIndex);
                            return {};
                        }
                        currentIndex += 1;
                        break;
                    }
                    case '<': {
                        if (currentIndex == 0) {
                            handler.makeError("Pointer out of bounds.", currentIndex);
                            return {};
                        }
                        currentIndex -= 1;
                        break;
                    }
                    case '+': {
                        tape[currentIndex] += 1;
                        break;
                    }
                    case '-': {
                        tape[currentIndex] -= 1;
                        break;
                    }
                    case '.': {
                        uint8_t value = tape[currentIndex];
                        res += static_cast<char>(value);
                        break;
                    }
                    case ',': {
                        int input = std::cin.get();
                        if (input == '\0') {
                            tape[currentIndex] = 0;
                        } else {
                            tape[currentIndex] = static_cast<uint8_t>(input);
                        }
                        break;
                    }
                    case '[': {
                        if (tape[currentIndex] == 0) {
                            auto it = jumpTable.find(i);
                            if (it != jumpTable.end()) {
                                i = it->second;
                            }
                        }
                        break;
                    }
                    case ']': {
                        if (tape[currentIndex] != 0) {
                            auto it = jumpTable.find(i);
                            if (it != jumpTable.end()) {
                                i = it->second;
                            }
                        }
                        break;
                    }
                    default: {
                        std::string warningText = "Invalid character catch: ";
                        warningText += cmd;
                        handler.makeWarning(warningText, i);
                        break;
                    }
                }
            }

            return res;
        }

        /**
         * @brief Helper function, transform the string to vector<uint8_t>
         *
         * This function transform the string to vector<uint8_t>, make it more
         * consistent.
         *
         * @param target The code string.
         * @return Transformed code.
         */
        std::vector<uint8_t> trans_string_to_vec(std::string target) {
            if (target.empty()) {
                handler.makeError("trans_string_to_vec() failed.", 0);
                return {};
            }

            std::vector<uint8_t> res;
            for (auto &i: target) {
                res.push_back(static_cast<uint8_t>(i));
            }

            return res;
        }

        /**
         * @brief Build the jump table, for processing the loop `[` and `]`
         *
         * We build the jump table, to navigating the loop. And I use the stack
         * for parsing the code, if we found `[`, push the position into stack.
         * If we found `]`, mapping the index for `[` and `]` and push into the map.
         * If any of the brackets is missing, make a error.
         *
         * @param code The code itself.
         * @return The jump table.
         */
        std::unordered_map<size_t, size_t> buildJumpTable(const std::vector<uint8_t> &code) {
            std::unordered_map<size_t, size_t> jumpTable;
            std::stack<size_t> stack;
            for (size_t i = 0; i < code.size(); ++i) {
                if (code[i] == '[') {
                    stack.push(i);
                } else if (code[i] == ']') {
                    if (stack.empty()) {
                        handler.makeError("Unmatched ']'", i);
                        return {};
                    }
                    auto start = stack.top();
                    stack.pop();
                    jumpTable[start] = i;
                    jumpTable[i] = start;
                }
            }

            while (!stack.empty()) {
                handler.makeError("Unmatched '['", stack.top());
                stack.pop();
            }

            return jumpTable;
        }

    private:
        size_t currentIndex = 0;
        std::vector<uint8_t> tape = std::vector<uint8_t>(30000, 0);
    };
}
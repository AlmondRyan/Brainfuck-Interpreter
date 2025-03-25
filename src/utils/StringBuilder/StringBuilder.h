#ifndef RIK_STRING_BUILDER_H
#define RIK_STRING_BUILDER_H

#include <string>
#include <sstream>

namespace Rikkyu::utils {
    class StringBuilder {
    public:
        StringBuilder() = default;
        ~StringBuilder() = default;

        template<typename... StringArgs>
        static std::string concatenate(StringArgs&&... stringArgs) {
            std::ostringstream finalResult;
            append(finalResult, std::forward<StringArgs>(stringArgs)...);
            return finalResult.str();
        }

    private:
        template<typename Tp, typename... StringArgs>
        static void append(std::ostringstream &res, Tp &&first, StringArgs &&... estimated) {
            res << std::forward<Tp>(first);
            if constexpr (sizeof...(estimated) > 0) {
                append(res, std::forward<StringArgs>(estimated)...);
            }
        }

        static void append(std::string &) {}
    };
}

#endif // RIK_STRING_BUILDER_H
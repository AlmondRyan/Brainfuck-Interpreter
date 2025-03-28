// Whitespace 解释器主程序 - 测试版本

#include "src/whitespace/interpreter.h"
#include "src/utils/ErrorHandler/ErrorHandler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace Rikkyu::Whitespace;

int main(int argc, char **argv) {
    // 使用固定的文件路径
    const std::string filePath = R"(D:\Rikkyu\test\whitespaces\hello-world.whs)";

    // 打开指定的文件
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "错误: 无法打开文件 " << filePath << std::endl;
        return 1;
    }

    std::cout << "Now original file start ===========" << std::endl;
    std::vector<char> t_code; char xc;
    while (file.get(xc)) {
        switch (xc) {
        case ' ':
            std::cout << "[Space]";
            break;
        case '\t':
            std::cout << "[Tab]";
            break;
        case '\n':
            std::cout << "[Line Feed]" << std::endl;
            break;
        default:
            continue;
        }
    }
    std::cout << "\nOriginal file end =================" << std::endl;

    // 读取文件内容
    std::vector<char> code;
    char c;
    while (file.get(c)) {
        code.push_back(c);
    }
    file.close();

    try {
        // 解析并执行Whitespace代码
        auto expressions = Parser().parse(code);
        if (!expressions.empty()) {
            Runner runner;
            runner.run(expressions);
        }
        // 输出所有错误和警告
        Rikkyu::utils::ErrorHandler::getInstance().printErrors();
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }

    return 0;
}
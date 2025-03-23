#include <iostream>
#include "src/brainfuck/interpreter.h"

using namespace Rikkyu::Brainfuck;

int main(int argc, char **argv) {
    std::string str; std::getline(std::cin, str);

    std::vector<char> prog;
    for (auto i : str) {
        prog.push_back(i);
    }

    try {
        auto expressions = Parser().parse(prog);
        if (!expressions.empty()) {
            Runner runner;
            runner.run(expressions);
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
#include <iostream>
#include "interpreter.hpp"
#include "utils.hpp"

namespace YAOBIWrapper {
    YAOBI::Version version;
    void initData() {
        version = YAOBI::Version(1, 0, 0);
    }

    void displayInfos() {
        initData();
        std::cout << "YAOBI (Yet Another Optimized Brainfuck Interpreter)" << std::endl;
        std::string versionStr = std::to_string(version.major) + "." + std::to_string(version.minor) +
                "." + std::to_string(version.patch) + version.preRelease + version.buildMetadata;
        std::string systemVersion = YAOBI::GetSysVersion(), systemBit = std::to_string(YAOBI::GetSysBit());
        std::cout << "Version: " << versionStr << " - " << systemVersion << "[" << systemBit << "]" << std::endl;
        std::cout << "Copyright (c) 2025 Ryan \"Nvkopres\" Almond, All rights reserved." << std::endl;
        std::cout << "This project licensed under MIT License." << std::endl;
        std::cout << std::endl;
    }
}


int main(int argc, char **argv) {
    YAOBIWrapper::displayInfos();
    std::string code, res;
    std::getline(std::cin, code);

    BrainfuckInterpreter::BrainfuckInterpreter bfitp;
    res = bfitp.interpret(code);

    std::cout << res << std::endl;
    return 0;
}
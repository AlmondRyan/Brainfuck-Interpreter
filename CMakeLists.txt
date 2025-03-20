cmake_minimum_required(VERSION 3.28)
project(bfInterpreter)

set(CMAKE_CXX_STANDARD 20)

add_executable(bfInterpreter main.cpp
        interpreter.hpp
        utils.hpp)

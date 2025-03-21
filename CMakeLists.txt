cmake_minimum_required(VERSION 3.28)
project(bfInterpreter)

set(CMAKE_CXX_STANDARD 20)

add_executable(Rikyuu main.cpp
        includes/defs/defs.hpp)

add_subdirectory(src/)
include_directories(includes/)
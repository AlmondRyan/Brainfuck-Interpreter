cmake_minimum_required(VERSION 3.28)
project(bfInterpreter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Rikkyu main.cpp
        includes/defs/defs.hpp)

add_subdirectory(src/)
include_directories(includes/)

target_link_libraries(Rikkyu PRIVATE Rikkyu_Source)
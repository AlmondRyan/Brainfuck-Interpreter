add_library(Rikkyu_Source
        brainfuck/interpreter.cpp
        brainfuck/interpreter.h
        utils/ErrorHandler/ErrorHandler.cpp
        utils/ErrorHandler/ErrorHandler.h
        utils/ConsoleTextManager/ConsoleTextManager.h
        brainfuck/AbstractExpression.h
)

target_include_directories(Rikkyu_Source PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../includes
        ./
)

include_directories(
        ./
)
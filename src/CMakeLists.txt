add_library(Rikkyu_Source
        # Brainfuck
        brainfuck/AbstractExpression.h
        brainfuck/interpreter.h

        # Whitespace
        whitespace/interpreter.h
        whitespace/AbstractExpression.h
        whitespace/expressions/ArithmeticExpressions.h
        whitespace/expressions/FlowExpressions.h
        whitespace/expressions/HeapExpressions.h
        whitespace/expressions/IOExpressions.h
        whitespace/expressions/StackExpressions.h

        # Utility
        utils/ErrorHandler/ErrorHandler.cpp
        utils/ErrorHandler/ErrorHandler.h
        utils/ConsoleTextManager/ConsoleTextManager.h
        utils/StringBuilder/StringBuilder.h
        whitespace/Runner.cpp
)

target_include_directories(Rikkyu_Source PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../includes
        ./
)

include_directories(
        ./
)
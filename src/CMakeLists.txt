cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_custom_command(
        OUTPUT run-clang-format
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND cd scripts && python3 run-clang-format.py ../src/csmt.h
        COMMENT "Run clang format"
)

add_custom_target(format
        DEPENDS run-clang-format
        COMMENT "Finish clang format"
        )

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic -Wextra")
endif()

add_executable(dummy_exec main.cpp ${CRYPTO_SRC} csmt.h)

# add_dependencies(dummy_exec format)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

add_executable(tmp_exec main.cpp ${CRYPTO_SRC} csmt.h)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic -Wextra")
endif()

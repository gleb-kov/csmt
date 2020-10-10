cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_executable(unit_tests ut.cpp)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic -O2")
    # set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -flto")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -ggdb -fsanitize=address,undefined,leak")
endif()

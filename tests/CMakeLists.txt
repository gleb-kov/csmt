cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_library(gtest STATIC ${GTEST_SRC})

message("-- Configuring tests:")
foreach (TEST_TYPE unit stress structural)
    message("   - ${TEST_TYPE}")
    add_executable(${TEST_TYPE}_tests ${TEST_TYPE}_tests.cpp)
    target_link_libraries(${TEST_TYPE}_tests gtest)
    if (UNIX AND NOT APPLE)
        target_link_libraries(${TEST_TYPE}_tests pthread)
    endif (UNIX AND NOT APPLE)
endforeach ()

add_executable(emulate common/emulate.cpp)
message("-- Emulator built")

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic -O2")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
    if (NOT MINGW)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -fsanitize=address,undefined,leak")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
    endif ()
endif ()
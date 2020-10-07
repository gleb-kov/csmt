cmake_minimum_required(VERSION 3.10)
project(csmt)

set(CMAKE_CXX_STANDARD 17)

set(GTEST_DIR ${PROJECT_SOURCE_DIR}/contrib/gtest)

set(GTEST_SRC
        ${GTEST_DIR}/gtest-all.cc
        ${GTEST_DIR}/gtest.h
        ${GTEST_DIR}/gtest_main.cc)

include_directories(.)

add_subdirectory(src)
add_subdirectory(tests)

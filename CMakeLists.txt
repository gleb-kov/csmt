cmake_minimum_required(VERSION 3.10)
project(csmt)

set(CMAKE_CXX_STANDARD 17)

#####################################################################

set(CONTRIB_DIR ${PROJECT_SOURCE_DIR}/contrib)

set(GTEST_DIR ${CONTRIB_DIR}/gtest)
set(GTEST_SRC
        ${GTEST_DIR}/gtest-all.cc
        ${GTEST_DIR}/gtest.h
        ${GTEST_DIR}/gtest_main.cc)

set(CRYPTO_DIR ${CONTRIB_DIR}/crypto)
set(CRYPTO_SRC
        ${CRYPTO_DIR}/sha256.h
        ${CRYPTO_DIR}/sha256.cpp)

#####################################################################

include_directories(.)
add_subdirectory(src)
add_subdirectory(tests)

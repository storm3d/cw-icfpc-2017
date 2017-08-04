cmake_minimum_required(VERSION 3.8)
project(cw_icfpc_2017)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp State.cpp State.h)
add_executable(cw_icfpc_2017 ${SOURCE_FILES})

set(SOURCE_FILES local_runner.cpp)
add_executable(local_runner ${SOURCE_FILES})

set(SOURCE_FILES tests/tests-main.cpp)
add_executable(tests ${SOURCE_FILES})

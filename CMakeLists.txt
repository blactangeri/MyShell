cmake_minimum_required(VERSION 3.3)
project(MyShell1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES MyShell.cpp)
add_executable(MyShell1 ${SOURCE_FILES})
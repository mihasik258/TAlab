cmake_minimum_required(VERSION 3.17.5)

set(CMAKE_CXX_STANDARD 20)

add_library(ast STATIC ast.cpp)

target_include_directories(ast PUBLIC .)

cmake_minimum_required(VERSION 3.17.5)

set(CMAKE_CXX_STANDARD 20)

add_library(ast_builder builder.cpp)

target_include_directories(ast_builder PUBLIC .)

target_link_libraries(ast_builder ast)

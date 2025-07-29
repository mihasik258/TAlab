cmake_minimum_required(VERSION 3.17.5)

set(CMAKE_CXX_STANDARD 20)

add_library(nfa NFA.cpp)

target_include_directories(nfa PUBLIC .)

target_link_libraries(nfa ast ast_builder)

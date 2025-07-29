cmake_minimum_required(VERSION 3.17.5)

set(CMAKE_CXX_STANDARD 20)

add_library(dfa DFA.cpp)

target_include_directories(dfa PUBLIC .)

target_link_libraries(dfa nfa ast ast_builder)

cmake_minimum_required(VERSION 3.17.5)

set(CMAKE_CXX_STANDARD 20)

add_library(match MatchResult.cpp)

target_include_directories(match PUBLIC .)

target_link_libraries(match dfa nfa ast ast_builder)

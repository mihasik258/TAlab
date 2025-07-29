cmake_minimum_required(VERSION 3.17.5)

set(CMAKE_CXX_STANDARD 20)

add_library(my_regex my_regex.cpp)

target_include_directories(my_regex PUBLIC .)

target_link_libraries(my_regex ast ast_builder nfa dfa)

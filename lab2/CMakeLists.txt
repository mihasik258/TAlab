cmake_minimum_required(VERSION 3.17.5)
project(lab2)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(ast)
add_subdirectory(ast_builder)
add_subdirectory(nfa)
add_subdirectory(dfa)
add_subdirectory(my_regex)
#add_subdirectory(match)

add_executable(main main.cpp tests/tests.cpp)

target_link_libraries(main gtest pthread ast ast_builder nfa dfa my_regex)

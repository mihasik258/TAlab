cmake_minimum_required(VERSION 3.17.5)
project(untitled5)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled main.cpp
        SmcRecognizer.cpp
        SmcRecognizer.h
        SmcRecognizer_sm.h
        SmcRecognizer_sm.cpp
        FlexRecognizer.h
        FlexRecognizer.cpp
        RegexRecognizer.h
        IMyRecognizer.h
        RegexRecognizer.cpp)

cmake_minimum_required(VERSION 3.21)
project(Project4)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Project4
        NameTable.h
        NameTable.cpp
        testNameTable.cpp
        )

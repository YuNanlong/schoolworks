cmake_minimum_required(VERSION 3.7)
project(list)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        list.cpp
        list.h
        main.cpp
        data.h)

add_executable(list ${SOURCE_FILES})
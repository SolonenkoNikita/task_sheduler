cmake_minimum_required(VERSION 3.22)

project(Logger)

set(CMAKE_CXX_STANDARD 20)

add_library (Logger STATIC source/Logger.cpp)

target_include_directories(Logger PUBLIC include)
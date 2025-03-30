cmake_minimum_required(VERSION 3.22)

project(Task)

set(CMAKE_CXX_STANDARD 20)

add_library (Task STATIC source/Task.cpp)

target_include_directories(Task PUBLIC include)
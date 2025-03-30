cmake_minimum_required(VERSION 3.22)
project(Tasks)

set(CMAKE_CXX_STANDARD 20)

add_library (Tasks STATIC source/Tasks.cpp)

target_link_libraries(Tasks Task)

target_include_directories(Tasks PUBLIC include)
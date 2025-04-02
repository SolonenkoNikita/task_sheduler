cmake_minimum_required(VERSION 3.22)

project(TaskSerialize)

set(CMAKE_CXX_STANDARD 20)

add_library(TaskSerialize INTERFACE)

#target_link_libraries(TaskSerialize INTERFACE Task Tasks)

target_include_directories(TaskSerialize INTERFACE include)
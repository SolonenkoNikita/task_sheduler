cmake_minimum_required(VERSION 3.22)

project(SharedMemory)

set(CMAKE_CXX_STANDARD 20)

add_library(SharedMemory INTERFACE)

target_link_libraries(SharedMemory INTERFACE Task)

target_include_directories(SharedMemory INTERFACE include)
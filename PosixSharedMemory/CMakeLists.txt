cmake_minimum_required(VERSION 3.22)

project(PosixSharedMemory)

set(CMAKE_CXX_STANDARD 20)

add_library (PosixSharedMemory STATIC source/PosixSharedMemory.cpp)

target_link_libraries(PosixSharedMemory SharedMemory Logger)

target_include_directories(PosixSharedMemory PUBLIC include)
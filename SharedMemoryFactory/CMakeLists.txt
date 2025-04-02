cmake_minimum_required(VERSION 3.22)

project(SharedMemoryFactory)

set(CMAKE_CXX_STANDARD 20)

add_library (SharedMemoryFactory INTERFACE)

target_link_libraries(SharedMemoryFactory INTERFACE PosinSharedMemory)

target_include_directories(SharedMemoryFactory INTERFACE include)
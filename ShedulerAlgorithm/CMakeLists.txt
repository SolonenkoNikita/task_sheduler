cmake_minimum_required(VERSION 3.22)
project(ShedulerAlgorithm)

set(CMAKE_CXX_STANDARD 20)

add_library (ShedulerAlgorithm INTERFACE)

target_link_libraries(ShedulerAlgorithm INTERFACE Task)

target_include_directories(ShedulerAlgorithm INTERFACE include)
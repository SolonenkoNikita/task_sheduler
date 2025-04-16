cmake_minimum_required(VERSION 3.22)
project(PriorityScheduling)

set(CMAKE_CXX_STANDARD 20)

add_library (PriorityScheduling STATIC source/PriorityScheduling.cpp)

target_link_libraries(PriorityScheduling ShedulerAlgorithm)

target_include_directories(PriorityScheduling PUBLIC include)
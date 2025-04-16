cmake_minimum_required(VERSION 3.22)
project(RoundRobinScheduling)

set(CMAKE_CXX_STANDARD 20)

add_library (RoundRobinScheduling STATIC source/RoundRobinScheduling.cpp)

target_link_libraries(RoundRobinScheduling ShedulerAlgorithm)

target_include_directories(RoundRobinScheduling PUBLIC include)
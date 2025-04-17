cmake_minimum_required(VERSION 3.22)

project(Sheduler)

set(CMAKE_CXX_STANDARD 20)

add_library (Sheduler STATIC source/Sheduler.cpp)

target_link_libraries(Sheduler PosixSharedMemory TaskQueueManager TaskProcessor ShedulerAlgorithm RoundRobinScheduling)

target_include_directories(Sheduler PUBLIC include)
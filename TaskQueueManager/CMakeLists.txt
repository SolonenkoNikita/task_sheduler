cmake_minimum_required(VERSION 3.22)
project(TaskQueueManager)

set(CMAKE_CXX_STANDARD 20)

add_library (TaskQueueManager STATIC source/TaskQueueManager.cpp)

target_link_libraries(TaskQueueManager PosixSharedMemory Task ShedulerAlgorithm Tasks)

target_include_directories(TaskQueueManager PUBLIC include)
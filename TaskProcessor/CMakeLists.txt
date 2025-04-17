cmake_minimum_required(VERSION 3.22)
project(TaskProcessor)

set(CMAKE_CXX_STANDARD 20)

add_library (TaskProcessor STATIC source/TaskProcessor.cpp)

target_link_libraries(TaskProcessor Logger TaskQueueManager)

target_include_directories(TaskProcessor PUBLIC include)
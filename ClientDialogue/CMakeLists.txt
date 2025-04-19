cmake_minimum_required(VERSION 3.22)
project(ClientDialogue)

set(CMAKE_CXX_STANDARD 20)

add_executable(ClientDialogue source/main.cpp)

target_link_libraries(ClientDialogue Sheduler Client Tasks)
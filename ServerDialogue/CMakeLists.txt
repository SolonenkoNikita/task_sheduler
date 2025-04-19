cmake_minimum_required(VERSION 3.22)
project(ServerDialogue)

set(CMAKE_CXX_STANDARD 20)

add_executable(ServerDialogue source/main.cpp)

target_link_libraries(ServerDialogue Server)
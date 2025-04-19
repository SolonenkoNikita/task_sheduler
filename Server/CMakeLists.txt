cmake_minimum_required(VERSION 3.22)

project(Server)

set(CMAKE_CXX_STANDARD 20)

add_library (Server STATIC source/Server.cpp)

target_link_libraries(Server Logger Sheduler Tasks)

target_include_directories(Server PUBLIC include)
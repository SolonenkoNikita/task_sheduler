cmake_minimum_required(VERSION 3.22)

project(Client)

set(CMAKE_CXX_STANDARD 20)

add_library (Client STATIC source/Client.cpp)

target_link_libraries(Client Logger)

target_include_directories(Client PUBLIC include)
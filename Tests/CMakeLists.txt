cmake_minimum_required(VERSION 3.22)
project(Tests)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)

add_executable (Tests   source/main.cpp
                        source/TestCaseTask.cpp)

target_link_libraries(Tests gtest
                            gtest_main
                            Task
)
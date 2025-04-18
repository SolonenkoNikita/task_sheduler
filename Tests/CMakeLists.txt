cmake_minimum_required(VERSION 3.22)
project(Tests)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)

find_package(GTest REQUIRED)
#find_package(GMock REQUIRED)

#include_directories(${GMOCK_INCLUDE_DIRS})

FetchContent_MakeAvailable(googletest)

add_executable (Tests   source/main.cpp
                        source/TestCaseTask.cpp
                        source/TestSharedMemory.cpp
                        source/TestQueueManager.cpp)

target_link_libraries(Tests gtest
                            gtest_main
                            Task
                            PosixSharedMemory
                            Tasks
                            TaskQueueManager
                            RoundRobinScheduling
                            PriorityScheduling
                            GTest::gmock
                            pthread
)
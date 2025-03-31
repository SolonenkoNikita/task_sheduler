cmake_minimum_required(VERSION 3.22)

project(TASK_SHEDULER)

add_definitions(-DLOGS_DIR="${PROJECT_SOURCE_DIR}/Logs")

add_subdirectory(Task)

add_subdirectory(Tests)

add_subdirectory(Tasks)

add_subdirectory(Logger)
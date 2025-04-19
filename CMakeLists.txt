cmake_minimum_required(VERSION 3.22)

project(TASK_SHEDULER)

add_definitions(-DLOGS_DIR="${PROJECT_SOURCE_DIR}/Logs")

add_subdirectory(Task)

add_subdirectory(Tests)

add_subdirectory(Tasks)

add_subdirectory(Logger)

add_subdirectory(SharedMemory)

add_subdirectory(PosixSharedMemory)

add_subdirectory(ShedulerAlgorithm)

add_subdirectory(RoundRobinScheduling)

add_subdirectory(PriorityScheduling)

add_subdirectory(TaskQueueManager)

add_subdirectory(TaskProcessor)

add_subdirectory(Sheduler)

add_subdirectory(ClientDialogue)

add_subdirectory(ServerDialogue)

add_subdirectory(Client)

add_subdirectory(Server)
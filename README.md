# Task Scheduler

## Overview

This project implements a **Task Scheduler** designed for Unix-based systems. It provides a robust and flexible framework for managing tasks in shared memory, with support for logging and error handling. The scheduler is implemented using modern C++ standards (C++20) to ensure high performance and maintainability. It works on the client-server principle (for adding tasks), since this is a model - only mathematical tasks. The client sends a command request to the server, the server processes the command and sends it to the scheduler.

## Features

- **Shared Memory Management**: Tasks are stored and managed in shared memory, allowing multiple processes to interact with the scheduler.
- **Task Prioritization**: Supports dynamic priority adjustment based on CPU usage, starvation, and I/O-bound characteristics.
- **Logging System**:
  - **State Logs**: Tracks the state of tasks and writes logs to `state_log`.
  - **Error Logs**: Records errors during program execution and writes logs to `error_log`.
- **Unix-Specific Features**: Utilizes Unix system calls such as `fork`, `exec`, and `waitpid` for process management.
- **C++20 Compliance**: Written using the latest C++20 standard, leveraging features like concepts, ranges, and improved chrono library.

## Requirements

- **Operating System**: Unix-based systems (Linux, macOS, etc.).
- **Compiler**: A C++20-compliant compiler (e.g., GCC 10+, Clang 12+).
- **Dependencies**:
  - CMake (for building the project).
  - Doxygen (optional, for generating documentation).

## Logging

The program includes a comprehensive logging system to monitor its operation:
- **State Logs (`state_log`)**: These logs track the lifecycle of tasks, including their creation, execution, and completion. This helps in debugging and monitoring task behavior.
- **Error Logs (`error_log`)**: Any errors encountered during the execution of the program are logged here. This ensures that issues can be traced and resolved efficiently.

Logs are stored in the `Logs/` directory:
Logs/
├── state_log
└── error_log
## Building and Running

### Prerequisites

1. Install a C++20-compliant compiler.
2. Install CMake:
   ```bash
   sudo apt install cmake
   ```
3. Generating Documentation (Optional)
   ```bash
   sudo apt install doxygen
   ```
   ```bash
   doxygen Doxyfile
   ```
   
## Documentation

You can find the project documentation [here](https://solonenkonikita.github.io/task_sheduler/).

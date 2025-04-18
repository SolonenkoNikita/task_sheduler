#pragma once

#include <Task/Task.hpp>

#include <thread>
#include <filesystem>
#include <fstream>

/**
 * @class CpuIntensiveTask
 * @brief Represents a CPU-intensive task.
 *
 * This class models a task that performs heavy computations, consuming significant CPU resources.
 */
class CpuIntensiveTask : public UnixTask
{
public:

    /**
     * @brief Constructs a `CpuIntensiveTask` instance.
     *
     * @param id The unique identifier for the task.
     * @param duration The total duration of work the task needs to perform.
     */
    CpuIntensiveTask(int, std::chrono::milliseconds);

    /**
     * @brief Executes the task for a given time quantum.
     *
     * Simulates CPU-intensive computation by performing mathematical
     * operations.
     *
     * @param quantum The maximum time the task can execute in this invocation.
     * @return bool True if the task is completed, false otherwise.
     */
    bool execute(std::chrono::milliseconds) override;

    std::chrono::milliseconds get_total_time() const noexcept override
    {
        return total_work_;
    }

  private:
    std::chrono::milliseconds total_work_;
    std::chrono::milliseconds remaining_work_;
};

/**
 * @class IoBoundTask
 * @brief Represents an I/O-bound task.
 *
 * This class models a task that primarily performs I/O operations, such as writing to files.
 */
class IoBoundTask : public UnixTask 
{
public:

    /**
     * @brief Constructs an `IoBoundTask` instance.
     *
     * @param id The unique identifier for the task.
     * @param file_path The path to the file where I/O operations will be performed.
     * @param operations The number of I/O operations the task needs to perform.
     */
    IoBoundTask(int, const std::string&, int);

    /**
     * @brief Executes the task for a given time quantum.
     *
     * Simulates I/O-bound operations by writing to a file and introducing
     * delays.
     *
     * @param quantum The maximum time the task can execute in this invocation.
     * @return bool True if the task is completed, false otherwise.
     */
    bool execute(std::chrono::milliseconds quantum) override;

private:
    std::filesystem::path file_path_;
    int operations_remaining_;
};
#pragma once

#include <Logger/Logger.hpp>
#include <TaskProcessor/TaskProcessor.hpp>
#include <TaskQueueManager/TaskQueueManager.hpp>
#include <RoundRobinScheduling/RoundRobingScheduling.hpp>

#define STATE_SCHEDULER "state_scheduler"

/**
 * @class Scheduler
 * @brief Manages task scheduling and processing.
 *
 * This class provides functionality to start and stop the scheduler, set
 * scheduling algorithms, add tasks to the queue, and manage the time quantum
 * for task execution.
 */
class Scheduler final
{
public:
    /**
     * @brief Constructs a Scheduler instance.
     *
     * Initializes the TaskQueueManager, TaskProcessor, and default scheduling algorithm (RoundRobinScheduling).
     *
     * @param shm Shared pointer to the PosixSharedMemory object.
     */
    Scheduler(std::shared_ptr<PosixSharedMemory> shm): 
        queue_manager_(std::make_shared<TaskQueueManager>(shm)),
        processor_(std::make_shared<TaskProcessor>(queue_manager_, std::chrono::milliseconds(100))),
        current_algorithm_(std::make_unique<RoundRobinScheduling>()),
        shm_(shm), running_(false), logger_(std::make_shared<ErrorLogger>(LOGS_DIR, STATE_SCHEDULER)){}

    /**
     * @brief Starts the scheduler and its associated components.
     *
     * Launches the task processor and starts the scheduling thread.
     */
    void start();

    /**
     * @brief Stops the scheduler and its associated components.
     *
     * Safely stops the task processor and joins the scheduling thread.
     */
    void stop();

    /**
     * @brief Sets a new scheduling algorithm.
     *
     * @param algorithm Unique pointer to the new SchedulingAlgorithm.
     */
    void set_algorithm(std::unique_ptr<SchedulingAlgorithm>);

    /**
     * @brief Adds a task to the task queue.
     *
     * Enqueues the task and reorders the queue based on the current scheduling algorithm.
     *
     * @param task Shared pointer to the GeneralTask to be added.
     */
    void add_task(std::shared_ptr<GeneralTask>);

    /**
     * @brief Retrieves the current number of tasks in the queue.
     *
     * @return size_t The number of tasks currently in the queue.
     */
    [[nodiscard]] inline size_t get_count() const noexcept
    {
        return queue_manager_->task_count();
    }

    /**
     * @brief Sets a new time quantum for task execution.
     *
     * @param quantum The new time quantum value.
     */
    void set_time_quantum(std::chrono::milliseconds);

private:
    std::shared_ptr<TaskQueueManager> queue_manager_;
    std::shared_ptr<TaskProcessor> processor_;
    std::shared_ptr<SchedulingAlgorithm> current_algorithm_;
    std::shared_ptr<PosixSharedMemory> shm_;
    std::atomic<bool> running_;
    std::thread scheduler_thread_;
    std::shared_ptr<Logger> logger_;

    /**
     * @brief Main scheduling loop.
     *
     * Continuously reorders tasks in the queue based on the current scheduling
     * algorithm.
     */
    void schedule();
};
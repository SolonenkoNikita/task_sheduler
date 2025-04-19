#pragma once

#include <TaskQueueManager/TaskQueueManager.hpp>
#include <Logger/Logger.hpp>

#include <atomic>
#include <memory>
#include <thread>

#define STATE_DIR "state_processor"

/**
 * @class TaskProcessor
 * @brief Manages the execution of tasks from a TaskQueueManager.
 *
 * This class retrieves tasks from the TaskQueueManager, executes them within a
 * specified time quantum, and re-adds incomplete tasks back to the queue. It
 * also provides methods to start, stop, and adjust the time quantum.
 */
class TaskProcessor final
{
public:
    /**
     * @brief Constructs a TaskProcessor instance.
     *
     * @param queue_manager Shared pointer to the TaskQueueManager from which tasks are retrieved.
     * @param time_quantum The initial time quantum for task execution.
     */
    TaskProcessor(std::shared_ptr<TaskQueueManager> queue_manager, std::chrono::milliseconds time_quantum)
        :queue_manager_(queue_manager), time_quantum_(time_quantum), running_(false),
        logger_(std::make_shared<FileLogger>(LOGS_DIR, STATE_DIR)) {}
    
    /**
     * @brief Starts the task processing thread.
     *
     * Launches a background thread that continuously processes tasks from the queue.
     */
    void start();

    /**
     * @brief Stops the task processing thread.
     *
     * Safely stops the processing loop and joins the background thread.
     */
    void stop();

    /**
     * @brief Sets a new time quantum for task execution.
     *
     * @param quantum The new time quantum value.
     */
    void set_time_quantum(std::chrono::milliseconds);

    /**
     * @brief Checks if the TaskProcessor is currently running.
     *
     * @return bool True if the processor is running, false otherwise.
     */
    [[nodiscard]] inline bool is_running() const noexcept
    {
        return running_;
    }

    /**
     * @brief Retrieves the TaskQueueManager associated with this processor.
     *
     * @return std::shared_ptr<TaskQueueManager> Shared pointer to the TaskQueueManager.
     */
    [[nodiscard]] inline std::shared_ptr<TaskQueueManager> get_queue_manager() const
    {
        return queue_manager_;
    }

private:
    std::shared_ptr<TaskQueueManager> queue_manager_;
    std::chrono::milliseconds time_quantum_;
    std::atomic<bool> running_;
    std::thread processor_thread_;
    std::shared_ptr<Logger> logger_;

    /**
     * @brief Processes tasks in a loop.
     *
     * Continuously retrieves tasks from the queue, executes them within the
     * time quantum, and handles incomplete tasks by re-adding them to the
     * queue.
     */
    void process_tasks();
};
#pragma once

#include <TaskQueueManager/TaskQueueManager.hpp>
#include <Logger/Logger.hpp>

#include <atomic>
#include <memory>
#include <thread>

#define STATE_DIR "state_processor"

class TaskProcessor final
{
public:
    TaskProcessor(std::shared_ptr<TaskQueueManager> queue_manager, std::chrono::milliseconds time_quantum)
        :queue_manager_(queue_manager), time_quantum_(time_quantum), running_(false),
        logger_(std::make_shared<FileLogger>(LOGS_DIR, STATE_DIR)) {}
    
    void start();
    
    void stop();
    
    void set_time_quantum(std::chrono::milliseconds);

    [[nodiscard]] inline bool is_running() const noexcept
    {
        return running_;
    }

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
    
    void process_tasks();
};
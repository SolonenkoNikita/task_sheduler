#pragma once

#include <TaskQueueManager/TaskQueueManager.hpp>
#include <Logger/Logger.hpp>

#include <atomic>
#include <memory>
#include <thread>

class TaskProcessor final
{
public:
    TaskProcessor(std::shared_ptr<TaskQueueManager> queue_manager, std::chrono::milliseconds time_quantum)
        :queue_manager_(queue_manager), time_quantum_(time_quantum), running_(false),
        logger_(std::make_shared<FileLogger>("logs", "processor.log")) {}
    
    void start();
    
    void stop();
    
    void set_time_quantum(std::chrono::milliseconds);

private:
    std::shared_ptr<TaskQueueManager> queue_manager_;
    std::chrono::milliseconds time_quantum_;
    std::atomic<bool> running_;
    std::thread processor_thread_;
    std::shared_ptr<Logger> logger_;
    
    void process_tasks();
};
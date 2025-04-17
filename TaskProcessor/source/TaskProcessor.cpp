#include "TaskProcessor/TaskProcessor.hpp"

void TaskProcessor::start() 
{
    running_ = true;
    processor_thread_ = std::thread(&TaskProcessor::process_tasks, this);
}

void TaskProcessor::stop() 
{
    running_ = false;
    if (processor_thread_.joinable()) 
        processor_thread_.join();
}

void TaskProcessor::set_time_quantum(std::chrono::milliseconds quantum) 
{
    time_quantum_ = quantum;
}

void TaskProcessor::process_tasks() 
{
    while (running_) 
    {
        try 
        {
        if (queue_manager_->task_count() > 0) 
        {
            auto task = queue_manager_->get_next_task();

            logger_->log("Processing task: " + task->get_description());

            bool completed = task->execute(time_quantum_);

            if (!completed)
                queue_manager_->add_task(task);
            else 
                logger_->log("Task completed: " + task->get_description());
        } 
        else 
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } 
        catch (const std::exception &e) 
        {
            logger_->log("Error processing task: " + std::string(e.what()));
        }
    }
}
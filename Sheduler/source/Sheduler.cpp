#include "Sheduler/Sheduler.hpp"

void Scheduler::start() 
{
    logger_->log("Starting scheduler...");
    shm_->set_scheduler_running(true);
    running_ = true;
    logger_->log("Starting task processor...");

    processor_->start();

    logger_->log("Starting scheduler thread...");
    scheduler_thread_ = std::thread(&Scheduler::schedule, this);
}

void Scheduler::stop() 
{
    shm_->set_scheduler_running(false);
    running_ = false;

    processor_->stop();
    if (scheduler_thread_.joinable()) 
        scheduler_thread_.join();
}

void Scheduler::set_algorithm(std::unique_ptr<SchedulingAlgorithm> algorithm) 
{
    current_algorithm_ = std::move(algorithm);
}

void Scheduler::add_task(std::shared_ptr<GeneralTask> task) 
{
    queue_manager_->add_task(task);
    queue_manager_->reorder_tasks(current_algorithm_);
}

void Scheduler::set_time_quantum(std::chrono::milliseconds quantum) 
{
    processor_->set_time_quantum(quantum);
}

void Scheduler::schedule() 
{
    while (running_) 
    {
        try 
        {
            queue_manager_->reorder_tasks(current_algorithm_);
                
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } 
        catch (const std::exception& e) 
        {
            logger_->log(std::string(e.what()));
        }
    }
}
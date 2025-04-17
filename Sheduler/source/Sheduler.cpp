#include "Sheduler/Sheduler.hpp"

void Scheduler::start() 
{
    shm_->set_scheduler_running(true);
    running_ = true;

    processor_->start();
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
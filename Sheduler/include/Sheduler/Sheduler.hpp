#pragma once

#include <Logger/Logger.hpp>
#include <TaskProcessor/TaskProcessor.hpp>
#include <TaskQueueManager/TaskQueueManager.hpp>
#include <RoundRobinScheduling/RoundRobingScheduling.hpp>

class Scheduler final
{
public:
    Scheduler(std::shared_ptr<PosixSharedMemory> shm): 
        queue_manager_(std::make_shared<TaskQueueManager>(shm)),
        processor_(std::make_shared<TaskProcessor>(queue_manager_, std::chrono::milliseconds(100))),
        current_algorithm_(std::make_unique<RoundRobinScheduling>()),
        running_(false),
        logger_(std::make_shared<ErrorLogger>(LOGS_DIR, STATE_DIR)){}
    
    void start();
    
    void stop();
    
    void set_algorithm(std::unique_ptr<SchedulingAlgorithm>);
    
    void add_task(std::shared_ptr<GeneralTask>);
    
    void set_time_quantum(std::chrono::milliseconds);

private:
    std::shared_ptr<TaskQueueManager> queue_manager_;
    std::shared_ptr<TaskProcessor> processor_;
    std::shared_ptr<SchedulingAlgorithm> current_algorithm_;
    std::shared_ptr<PosixSharedMemory> shm_;
    std::atomic<bool> running_;
    std::thread scheduler_thread_;
    std::shared_ptr<Logger> logger_;

    void schedule() 
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
                // Логирование ошибки
            }
        }
    }
};
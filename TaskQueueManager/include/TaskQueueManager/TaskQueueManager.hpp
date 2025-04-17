#pragma once

#include <Task/Task.hpp>
#include <PosixSharedMemory/PosixSharedMemory.hpp>
#include <ShedulerAlgorithm/ShedulerAlgorithm.hpp>

class TaskQueueManager final
{
public:
    explicit TaskQueueManager(std::shared_ptr<PosixSharedMemory> shm): shared_memory_(shm) {}
    
    void add_task(std::shared_ptr<GeneralTask>);
    
    std::shared_ptr<GeneralTask> get_next_task();

    [[nodiscard]] inline size_t task_count() const 
    {
        return shared_memory_->size();
    }
    
    void reorder_tasks(std::shared_ptr<SchedulingAlgorithm>);

private:
    std::shared_ptr<PosixSharedMemory> shared_memory_;
    
private:
    void convert_to_shared_task(std::shared_ptr<GeneralTask>, SharedTask&);
    
    std::shared_ptr<GeneralTask> convert_from_shared_task(const SharedTask&);
};
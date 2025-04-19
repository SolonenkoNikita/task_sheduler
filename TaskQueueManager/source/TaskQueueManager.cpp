#include "TaskQueueManager/TaskQueueManager.hpp"

void TaskQueueManager::reorder_tasks(std::shared_ptr<SchedulingAlgorithm> algorithm) 
{
    std::vector<std::shared_ptr<GeneralTask>> tasks;
        
    while (!shared_memory_->empty()) 
        tasks.emplace_back(get_next_task());
        
    for (auto& task : tasks) 
            algorithm->update_task_priority(task);
        
    for (auto& task : tasks) 
        add_task(task);
}

void TaskQueueManager::add_task(std::shared_ptr<GeneralTask> task) 
{
    SharedTask st;
    convert_to_shared_task(task, st);
    shared_memory_->enqueue(st);
    shared_memory_->print();
}

std::shared_ptr<GeneralTask> TaskQueueManager::get_next_task() 
{
    SharedTask st = shared_memory_->dequeue();
    return convert_from_shared_task(st);
}

void TaskQueueManager::convert_to_shared_task(std::shared_ptr<GeneralTask> src, SharedTask& dst) 
{
    dst.id_ = src->get_id();
    dst.priority_ = src->get_priority();
    strncpy(dst.description_, src->get_description().c_str(),  sizeof(dst.description_) - 1);
    dst.description_[sizeof(dst.description_) - 1] = '\0';
    dst.completed_ = src->is_completed();
    if (dst.completed_) 
    {
        dst.remaining_time_ms_ = 0;
        return;
    }
    if (dynamic_cast<CpuIntensiveTask*>(src.get())) 
        dst.type_ = TaskType::CPU_INTENSIVE_TASK;
    else if (dynamic_cast<IoBoundTask*>(src.get())) 
        dst.type_ = TaskType::IO_BOUND_TASK;
    else 
        dst.type_ = TaskType::UNIX_TASK;
    auto arrival_time = src->get_arrival_time();
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - arrival_time).count();
    auto total_time_ms = src->get_total_time().count();
    dst.remaining_time_ms_ = std::max(0, static_cast<int> (total_time_ms - elapsed_time_ms));
}

std::shared_ptr<GeneralTask> TaskQueueManager::convert_from_shared_task(const SharedTask& src) 
{
    std::shared_ptr<UnixTask> task;

    switch (src.type_) 
    {
        case TaskType::CPU_INTENSIVE_TASK:
            task = std::make_shared<CpuIntensiveTask>(src.id_, std::chrono::milliseconds(src.remaining_time_ms_));
            break;

        case TaskType::IO_BOUND_TASK:
            task = std::make_shared<IoBoundTask>(src.id_, "output.txt", src.remaining_time_ms_); //just a flag
            break;

        case TaskType::UNIX_TASK:
        default:
            task = std::make_shared<UnixTask>(src.id_, src.description_);
            break;
    }

    task->set_static_priority(src.priority_);
    if (src.completed_) 
        task->set_state(UnixTask::TaskState::COMPLETED);

    return task;
}
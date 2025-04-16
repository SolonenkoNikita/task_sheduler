#include "PriorityScheduling/PriorityScheduling.hpp"

size_t PriorityScheduling::select_next_task(const std::vector<std::shared_ptr<GeneralTask>>& tasks)
{
    if (tasks.empty()) 
        throw std::runtime_error("No tasks available");
        
    size_t selected = 0;
    int highest_priority = tasks[0]->get_priority();
        
    for (size_t i = 1; i < tasks.size(); ++i) 
    {
        if (tasks[i]->get_priority() > highest_priority) 
        {
            highest_priority = tasks[i]->get_priority();
            selected = i;
        }
    }
        
    return selected;
}

void PriorityScheduling::update_task_priority(std::shared_ptr<GeneralTask>& task)
{
    task->adjust_dynamic_priority();
}
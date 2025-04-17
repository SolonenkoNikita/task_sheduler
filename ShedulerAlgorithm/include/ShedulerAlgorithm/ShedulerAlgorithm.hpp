#pragma once

#include <Task/Task.hpp>
#include <vector>

class SchedulingAlgorithm 
{
public:
    virtual ~SchedulingAlgorithm() = default;
    
    /**
     * @brief Selects the next task to execute
     * @param tasks List of available tasks
     * @return Index of selected task
     */
    virtual size_t select_next_task(const std::vector<std::shared_ptr<GeneralTask>>&) = 0;
    
    /**
     * @brief Updates task priorities if needed
     * @param task Task to update
     */
    virtual void update_task_priority(std::shared_ptr<GeneralTask>) = 0;
    
    /**
     * @brief Returns algorithm name
     */
    virtual std::string get_name() const = 0;
};
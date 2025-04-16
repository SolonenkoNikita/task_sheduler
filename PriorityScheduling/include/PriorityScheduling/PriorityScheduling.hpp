#pragma once

#include <ShedulerAlgorithm/ShedulerAlgorithm.hpp>

static const std::string PRIORITY_SCHEDULING = "Priority Scheduling";

/**
 * @class PriorityScheduling
 * @brief Implements the Priority Scheduling algorithm as a concrete subclass of SchedulingAlgorithm.
 *
 * This class provides an implementation of the Priority Scheduling algorithm, which selects tasks based on their priority.
 * The task with the highest priority is chosen for execution. Additionally, the algorithm supports dynamic priority adjustment
 * for tasks to ensure fairness or adaptability in scheduling.
 *
 * @details
 * - **Selection Logic**: The `select_next_task` method iterates through the list of available tasks and selects the task
 *   with the highest priority. If multiple tasks share the same highest priority, the first one encountered is selected.
 * - **Priority Adjustment**: The `update_task_priority` method adjusts the dynamic priority of a task using its internal logic.
 * - **Algorithm Name**: The name of the algorithm is returned as "Priority Scheduling".
 */
class PriorityScheduling : public SchedulingAlgorithm 
{
public:
    /**
     * @brief Selects the next task to execute based on priority.
     * 
     * This method iterates through the list of available tasks and selects the task with the highest priority.
     * If the task list is empty, a `std::runtime_error` is thrown.
     * 
     * @param tasks A vector of shared pointers to GeneralTask objects representing the available tasks.
     * @return The index of the selected task in the input vector.
     * @throws std::runtime_error if the task list is empty.
     */
    size_t select_next_task(const std::vector<std::shared_ptr<GeneralTask>>&) override;
    
     /**
     * @brief Updates the dynamic priority of a given task.
     * 
     * This method calls the `adjust_dynamic_priority` method of the provided task to modify its priority dynamically.
     * This ensures that tasks can be re-prioritized over time based on runtime conditions.
     * 
     * @param task A shared pointer to the GeneralTask object whose priority needs to be updated.
     */
    void update_task_priority(std::shared_ptr<GeneralTask>&) override;

    /**
     * @brief Returns the name of the scheduling algorithm.
     *
     * @return A string containing the name of the algorithm: "Priority Scheduling".
     */
    [[nodiscard]] inline std::string get_name() const override 
    { 
        return PRIORITY_SCHEDULING; 
    }
};
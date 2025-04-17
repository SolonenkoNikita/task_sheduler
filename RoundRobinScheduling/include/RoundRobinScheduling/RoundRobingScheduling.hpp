#pragma once

#include <ShedulerAlgorithm/ShedulerAlgorithm.hpp>

static const std::string ROUND_ROBIN = "Round Robin";

/**
 * @class RoundRobinScheduling
 * @brief Implements the Round Robin Scheduling algorithm as a concrete subclass of SchedulingAlgorithm.
 *
 * This class provides an implementation of the Round Robin Scheduling algorithm, which cycles through tasks in a circular
 * manner, ensuring fairness in task execution. Each task is given an equal opportunity to execute, regardless of its priority.
 *
 * @details
 * - **Selection Logic**: The `select_next_task` method uses a circular index to select the next task in the list. If the end
 *   of the list is reached, the selection wraps around to the beginning.
 * - **Priority Adjustment**: This algorithm does not adjust task priorities, so the `update_task_priority` method is a no-op.
 * - **Algorithm Name**: The name of the algorithm is returned as "Round Robin".
 */
class RoundRobinScheduling : public SchedulingAlgorithm 
{
public:
    /**
     * @brief Constructor for RoundRobinScheduling.
     * 
     * Initializes the current index to 0, which is used to track the position in the task list during selection.
     */
    RoundRobinScheduling() : current_index_(0) {}

    /**
     * @brief Selects the next task to execute in a round-robin fashion.
     * 
     * This method increments the current index modulo the size of the task list to ensure circular selection.
     * If the task list is empty, a `std::runtime_error` is thrown.
     * 
     * @param tasks A vector of shared pointers to GeneralTask objects representing the available tasks.
     * @return The index of the selected task in the input vector.
     * @throws std::runtime_error if the task list is empty.
     */
    size_t select_next_task(const std::vector<std::shared_ptr<GeneralTask>>&) override;

    /**
     * @brief No-op implementation for updating task priority.
     * 
     * Since Round Robin Scheduling does not use task priorities, this method does nothing.
     * 
     * @param task A shared pointer to the GeneralTask object (unused in this implementation).
     */
    void update_task_priority(std::shared_ptr<GeneralTask>) override {}

    /**
     * @brief Returns the name of the scheduling algorithm.
     *
     * @return A string containing the name of the algorithm: "Round Robin".
     */
    [[nodiscard]] inline std::string get_name() const override 
    { 
        return ROUND_ROBIN; 
    }

private:
    size_t current_index_;
};
#pragma once

#include <Task/Task.hpp>
#include <PosixSharedMemory/PosixSharedMemory.hpp>
#include <ShedulerAlgorithm/ShedulerAlgorithm.hpp>
#include <Tasks/Tasks.hpp>

/**
 * @class TaskQueueManager
 * @brief Manages tasks in a shared memory queue.
 *
 * The TaskQueueManager class is responsible for adding tasks to the queue,
 * retrieving the next task, reordering tasks using a scheduling algorithm, and
 * converting between shared tasks and general tasks.
 */
class TaskQueueManager final
{
public:
    /**
     * @brief Constructs a TaskQueueManager with a reference to PosixSharedMemory.
     *
     * @param shm Reference to the PosixSharedMemory object.
     */
    explicit TaskQueueManager(PosixSharedMemory& shm) : shared_memory_(std::make_shared<PosixSharedMemory>(shm)) {}

    /**
     * @brief Constructs a TaskQueueManager with a shared pointer to
     * PosixSharedMemory.
     *
     * @param shm Shared pointer to the PosixSharedMemory object.
     */
    explicit TaskQueueManager(std::shared_ptr<PosixSharedMemory> shm): shared_memory_(shm) {}

    /**
     * @brief Adds a task to the shared memory queue.
     *
     * Converts the GeneralTask to a SharedTask and enqueues it into the shared memory.
     *
     * @param task Shared pointer to the GeneralTask to be added.
     */
    void add_task(std::shared_ptr<GeneralTask>);

    /**
     * @brief Retrieves the next task from the shared memory queue.
     *
     * Dequeues a SharedTask from the shared memory and converts it back to a GeneralTask.
     *
     * @return Shared pointer to the retrieved GeneralTask.
     */
    std::shared_ptr<GeneralTask> get_next_task();

    /**
     * @brief Retrieves the current number of tasks in the queue.
     *
     * @return size_t The number of tasks currently in the queue.
     */
    [[nodiscard]] inline size_t task_count() const 
    {
        return shared_memory_->size();
    }

    /**
     * @brief Reorders tasks in the queue based on a scheduling algorithm.
     *
     * Removes all tasks from the queue, updates their priorities using the
     * provided scheduling algorithm, and re-adds them to the queue.
     *
     * @param algorithm Shared pointer to the SchedulingAlgorithm used for
     * reordering.
     */
    void reorder_tasks(std::shared_ptr<SchedulingAlgorithm>);

private:
    std::shared_ptr<PosixSharedMemory> shared_memory_;
    
private:
    /**
     * @brief Converts a GeneralTask to a SharedTask.
     *
     * Populates the SharedTask structure with data from the GeneralTask.
     *
     * @param src Shared pointer to the source GeneralTask.
     * @param dst Reference to the destination SharedTask.
     */
    void convert_to_shared_task(std::shared_ptr<GeneralTask>, SharedTask&);
    
    /**
     * @brief Converts a SharedTask back to a GeneralTask.
     *
     * Creates a GeneralTask object (or its derived type) from the SharedTask.
     *
     * @param src Reference to the source SharedTask.
     * @return Shared pointer to the converted GeneralTask.
     */
    std::shared_ptr<GeneralTask> convert_from_shared_task(const SharedTask&);
};
#pragma once

#include <Task/Task.hpp>

#include <vector>

#define MAX_PATH 256

/**
 * @struct SharedTask
 * @brief Represents a task stored in shared memory.
 *
 * This structure defines the layout of a task that is stored in shared memory.
 * It includes metadata such as task ID, priority, description, completion
 * status, and remaining execution time.
 */
struct SharedTask 
{
    int id_;
    int priority_;
    char description_[MAX_PATH];
    bool completed_;
    int remaining_time_ms_;
};

/**
 * @class SharedMemory
 * @brief Abstract base class defining the interface for shared memory
 * operations.
 *
 * This class serves as an abstract interface for managing shared memory. It
 * declares pure virtual methods for creating, attaching, detaching, and
 * destroying shared memory, as well as for enqueueing and dequeueing tasks.
 * Concrete implementations of this interface must provide the actual
 * functionality.
 */
class SharedMemory
{
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~SharedMemory() = default;

    /**
     * @brief Creates a new shared memory segment.
     *
     * This method should allocate and initialize the shared memory segment.
     */
    virtual void create() = 0;

    /**
     * @brief Attaches to an existing shared memory segment.
     *
     * This method should map the shared memory segment into the process's
     * address space.
     */
    virtual void attach() = 0;

    /**
     * @brief Detaches from the shared memory segment.
     *
     * This method should unmap the shared memory segment from the process's
     * address space.
     */
    virtual void detach() = 0;

    /**
     * @brief Destroys the shared memory segment.
     *
     * This method should release all resources associated with the shared
     * memory segment.
     */
    virtual void destroy() = 0;

    /**
     * @brief Enqueues a task into the shared memory.
     *
     * @param task The task to be added to the shared memory.
     */
    virtual void enqueue(const SharedTask& ) = 0;

    /**
     * @brief Dequeues a task from the shared memory.
     *
     * @return The task retrieved from the shared memory.
     */
    virtual SharedTask dequeue() = 0;

    /**
     * @brief Gets the current number of tasks in the shared memory.
     *
     * @return The number of tasks currently stored in the shared memory.
     */
    virtual size_t size() const = 0;

    /**
     * @brief Checks if the shared memory is empty.
     *
     * @return True if there are no tasks in the shared memory, false otherwise.
     */
    virtual bool empty() const = 0;

    /**
     * @brief Gets the name of the shared memory segment.
     *
     * @return The name of the shared memory segment.
     */
    virtual const std::string& name() const noexcept = 0;

    /**
     * @brief Gets the capacity of the shared memory segment.
     *
     * @return The maximum number of tasks that can be stored in the shared
     * memory.
     */
    virtual size_t capacity() const noexcept = 0;
};

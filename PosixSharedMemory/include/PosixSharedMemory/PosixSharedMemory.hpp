#pragma once

#include <SharedMemory/SharedMemory.hpp>
#include <Logger/Logger.hpp>

#include <atomic>
#include <cstring>
#include <fcntl.h>
#include <mutex>
#include <semaphore.h>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

#define COUNT_TASKS 100
#define THROW_VALUE 10000
#define ERROR_DIR "error_log"

/**
 * @class PosixSharedMemory
 * @brief Implements the SharedMemory interface using POSIX shared memory APIs.
 *
 * This class provides concrete implementations for managing shared memory using
 * POSIX APIs. It supports operations such as creating, attaching, detaching,
 * and destroying shared memory, as well as enqueueing and dequeueing tasks. It
 * also includes synchronization mechanisms (semaphores) to ensure thread-safe
 * access to the shared memory.
 */
class PosixSharedMemory : public SharedMemory 
{
public:

    /**
     * @brief Constructor for PosixSharedMemory.
     * 
     * Initializes the shared memory object with the given name and capacity.
     * 
     * @param name The name of the shared memory segment.
     * @param capacity The maximum number of tasks that can be stored in the shared memory.
     * @throws std::invalid_argument If the capacity is invalid (zero or exceeds THROW_VALUE).
     */
    PosixSharedMemory(const std::string&, size_t = 100);

    /**
     * @brief Destructor for PosixSharedMemory.
     *
     * Cleans up resources associated with the shared memory segment.
     */
    ~PosixSharedMemory() override;

     /**
     * @struct SharedMemoryLayout
     * @brief Defines the layout of the shared memory segment.
     * 
     * This structure represents the data layout of the shared memory segment. It includes
     * atomic variables for synchronization and an array of tasks.
     */
    #pragma pack(push, 1)
    struct SharedMemoryLayout 
    {
        std::atomic<size_t> front_;
        std::atomic<size_t> rear_;
        std::atomic<size_t> count_;
        std::atomic<bool> scheduler_running_;
        SharedTask tasks_[COUNT_TASKS];

        std::atomic<size_t> total_enqueued_;
        std::atomic<size_t> total_dequeued_;
    };
    #pragma pack(pop)

    /**
     * @brief Creates a new shared memory segment.
     *
     * Allocates and initializes the shared memory segment and associated
     * semaphores.
     * @throws std::runtime_error If shared memory or semaphore creation fails.
     */
    void create() override;

    /**
     * @brief Attaches to an existing shared memory segment.
     *
     * Maps the shared memory segment into the process's address space.
     * @throws std::runtime_error If attachment fails.
     */
    void attach() override;

    /**
     * @brief Detaches from the shared memory segment.
     *
     * Unmaps the shared memory segment from the process's address space.
     */
    void detach() override;

    /**
     * @brief Destroys the shared memory segment.
     *
     * Releases all resources associated with the shared memory segment and semaphores.
     */
    void destroy() override;

    /**
     * @brief Enqueues a task into the shared memory.
     *
     * Adds a task to the shared memory queue in a thread-safe manner.
     *
     * @param task The task to be enqueued.
     * @throws std::runtime_error If semaphore operations fail.
     */
    void enqueue(const SharedTask &task) override;

    /**
     * @brief Dequeues a task from the shared memory.
     *
     * Removes and returns a task from the shared memory queue in a thread-safe
     * manner.
     *
     * @return The dequeued task.
     * @throws std::runtime_error If the queue is empty or semaphore operations fail.
     */
    SharedTask dequeue() override;

    /**
     * @brief Gets the current number of tasks in the shared memory.
     *
     * @return The number of tasks currently stored in the shared memory.
     */
    [[nodiscard]] inline size_t size() const override 
    { 
       return data_->count_.load(std::memory_order_relaxed);
    }

    /**
     * @brief Sets the scheduler running status.
     *
     * Updates the scheduler_running_ flag in the shared memory.
     *
     * @param running The new status of the scheduler (true if running, false
     * otherwise).
     */
    inline void set_scheduler_running(bool running) 
    {
        if (data_) 
            data_->scheduler_running_.store(running, std::memory_order_release);
    }

    /**
     * @brief Checks if the scheduler is running.
     *
     * @return True if the scheduler is running, false otherwise.
     */
    [[nodiscard]] inline bool is_scheduler_running() const 
    {
        return data_ && data_->scheduler_running_.load(std::memory_order_acquire);
    }

    /**
     * @brief Checks if the shared memory is empty.
     *
     * @return True if there are no tasks in the shared memory, false otherwise.
     */
    [[nodiscard]] inline bool empty() const override 
    { 
        return size() == 0; 
    }

    /**
     * @brief Gets the name of the shared memory segment.
     *
     * @return The name of the shared memory segment.
     */
    [[nodiscard]] inline const std::string& name() const noexcept override 
    { 
        return name_; 
    }

    /**
     * @brief Gets the capacity of the shared memory segment.
     *
     * @return The maximum number of tasks that can be stored in the shared
     * memory.
     */
    [[nodiscard]] inline size_t capacity() const noexcept override 
    { 
        return capacity_; 
    }

    void print();

private:

    /**
     * @brief Cleans up resources associated with the shared memory.
     * 
     * Attempts to destroy the shared memory segment and log any errors.
     */
    void cleanup();

    /**
     * @brief Validates the state of the shared memory.
     *
     * Ensures that the shared memory is properly attached.
     *
     * @throws std::runtime_error If the shared memory is not attached.
     */
    void validate() const;

    std::string name_;
    size_t capacity_;
    int fd_;
    SharedMemoryLayout* data_;
    
    sem_t* enqueue_sem_;
    sem_t* dequeue_sem_;
    sem_t* mutex_sem_;

    std::shared_ptr<Logger> logger_error_;
    std::shared_ptr<Logger> logger_state_;
};
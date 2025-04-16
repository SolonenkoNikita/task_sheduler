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

class PosixSharedMemory : public SharedMemory 
{
public:
    PosixSharedMemory(const std::string&, size_t = 100);
    ~PosixSharedMemory() override;

    #pragma pack(push, 1)
    struct SharedMemoryLayout 
    {
        std::atomic<size_t> front_;
        std::atomic<size_t> rear_;
        std::atomic<size_t> count_;
        std::atomic<bool> scheduler_running_;
        SharedTask tasks_[1]; //flexible value

        std::atomic<size_t> total_enqueued_;
        std::atomic<size_t> total_dequeued_;
    };
    #pragma pack(pop)

    void create() override;
    void attach() override;
    void detach() override;
    void destroy() override;

    void enqueue(const SharedTask &task) override;
    
    SharedTask dequeue() override;
    
    [[nodiscard]] inline size_t size() const override 
    { 
       return data_ ? data_->count_.load(std::memory_order_relaxed) : 0;
    }

    inline void set_scheduler_running(bool running) 
    {
        if (data_) 
            data_->scheduler_running_.store(running, std::memory_order_release);
    }

    [[nodiscard]] inline bool is_scheduler_running() const 
    {
        return data_ && data_->scheduler_running_.load(std::memory_order_acquire);
    }

    [[nodiscard]] inline bool empty() const override 
    { 
        return size() == 0; 
    }

    [[nodiscard]] inline const std::string& name() const noexcept override 
    { 
        return name_; 
    }

    [[nodiscard]] inline size_t capacity() const noexcept override 
    { 
        return capacity_; 
    }

private:
    void cleanup();
    void validate() const;

    std::string name_;
    size_t capacity_;
    int fd_;
    SharedMemoryLayout* data_;
    
    sem_t* enqueue_sem_;
    sem_t* dequeue_sem_;
    sem_t* mutex_sem_;

    std::shared_ptr<Logger> logger_;
};
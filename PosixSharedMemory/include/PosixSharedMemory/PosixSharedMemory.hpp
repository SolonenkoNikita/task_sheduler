#pragma once

#include <SharedMemory/SharedMemory.hpp>
#include <Logger/Logger.hpp>

#include <cstring>
#include <fcntl.h>
#include <mutex>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

#define COUNT_TASKS 100
#define ERROR_DIR "error_log"

class PosixSharedMemory : public SharedMemory 
{
public:
    PosixSharedMemory(const std::string&, size_t = 100);
    ~PosixSharedMemory() override;

    #pragma pack(push, 1)
    struct SharedMemoryLayout 
    {
        size_t front_;
        size_t rear_;
        size_t count_;
        bool scheduler_running_;
        SharedTask tasks_[COUNT_TASKS]; 

        size_t total_enqueued_;
        size_t total_dequeued_;
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
        return data_ ? data_->count_ : 0; 
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
    mutable std::mutex mutex_;

    std::shared_ptr<Logger> logger_;
};
#include "PosixSharedMemory/PosixSharedMemory.hpp"

#include <cstring>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

PosixSharedMemory::PosixSharedMemory(const std::string& name, size_t capacity)
    : name_(name), capacity_(capacity), fd_(-1), data_(nullptr),
    enqueue_sem_(SEM_FAILED), dequeue_sem_(SEM_FAILED), mutex_sem_(SEM_FAILED)
{
    if (capacity == 0 || capacity > THROW_VALUE)
        throw std::invalid_argument("Invalid value of capacity");
    logger_error_ = std::make_shared<ErrorLogger>(LOGS_DIR, ERROR_DIR);
    logger_state_ = std::make_shared<FileLogger>(LOGS_DIR, STATE_DIR);
}

PosixSharedMemory::~PosixSharedMemory() 
{
    cleanup();
}

void PosixSharedMemory::create() 
{
    size_t total_size = sizeof(SharedMemoryLayout) + (capacity_ - 1) * sizeof(SharedTask);

    sem_unlink(std::string("/" + name_ + "_enq").c_str());
    sem_unlink(std::string("/" + name_ + "_deq").c_str());
    sem_unlink(std::string("/" + name_ + "_mut").c_str());

    shm_unlink(name_.c_str());
    fd_ = shm_open(name_.c_str(), O_CREAT | O_RDWR | O_EXCL, 0666);
    if (fd_ == -1) 
        throw std::runtime_error("Failed to create shared memory: " + std::string(strerror(errno)));

    if (ftruncate(fd_, total_size) == -1) 
    {
        close(fd_);
        throw std::runtime_error("Ftruncate failed: " + std::string(strerror(errno)));
    }

    attach();

    data_->front_.store(0);
    data_->rear_.store(0);
    data_->count_.store(0);
    data_->scheduler_running_.store(false);
    data_->total_enqueued_.store(0);
    data_->total_dequeued_.store(0);

    enqueue_sem_ = sem_open(std::string("/" + name_ + "_enq").c_str(), O_CREAT | O_EXCL, 0666, capacity_);
    dequeue_sem_ = sem_open(std::string("/" + name_ + "_deq").c_str(), O_CREAT | O_EXCL, 0666, 0);
    mutex_sem_ = sem_open(std::string("/" + name_ + "_mut").c_str(), O_CREAT | O_EXCL, 0666, 1);

    if (enqueue_sem_ == SEM_FAILED || dequeue_sem_ == SEM_FAILED || mutex_sem_ == SEM_FAILED)
        throw std::runtime_error("Failed to create semaphores");
}

void PosixSharedMemory::attach() 
{
    if (fd_ == -1) 
    {
        fd_ = shm_open(name_.c_str(), O_RDWR, 0666);
        if (fd_ == -1) 
            throw std::runtime_error("shm_open failed: " + std::string(strerror(errno)));
    }

    size_t total_size = sizeof(SharedMemoryLayout) + (capacity_ - 1) * sizeof(SharedTask);

    data_ = static_cast<SharedMemoryLayout*>(mmap(nullptr, total_size,
                    PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0));

    if (data_ == MAP_FAILED) 
        throw std::runtime_error("mmap failed: " + std::string(strerror(errno)));
}

void PosixSharedMemory::detach() 
{
    if (data_) 
    {
        munmap(data_, sizeof(SharedMemoryLayout) + (capacity_ - 1) * sizeof(SharedTask));
        data_ = nullptr;
    }
}

void PosixSharedMemory::destroy() 
{
    detach();
    if (fd_ != -1) 
    {
        close(fd_);
        shm_unlink(name_.c_str());
        fd_ = -1;
    }

    if (enqueue_sem_ != SEM_FAILED) 
        sem_close(enqueue_sem_);
    if (dequeue_sem_ != SEM_FAILED) 
        sem_close(dequeue_sem_);
    if (mutex_sem_ != SEM_FAILED) 
        sem_close(mutex_sem_);

    sem_unlink(std::string("/" + name_ + "_enq").c_str());
    sem_unlink(std::string("/" + name_ + "_deq").c_str());
    sem_unlink(std::string("/" + name_ + "_mut").c_str());
}

void PosixSharedMemory::enqueue(const SharedTask& task) 
{
    if (sem_wait(enqueue_sem_) == -1) 
        throw std::runtime_error("Enqueue semaphore wait failed");
    if (sem_wait(mutex_sem_) == -1) 
    {
        sem_post(enqueue_sem_);
        throw std::runtime_error("Mutex semaphore wait failed");
    }

    size_t rear = data_->rear_.load(std::memory_order_relaxed);
    data_->tasks_[rear] = task;
    data_->rear_.store((rear + 1) % capacity_, std::memory_order_relaxed);
    data_->count_.fetch_add(1, std::memory_order_relaxed);
    data_->total_enqueued_.fetch_add(1, std::memory_order_relaxed);

    logger_state_->log("Enqueued task with id: " + std::to_string(task.id_));

    sem_post(mutex_sem_);
    sem_post(dequeue_sem_);
}

void PosixSharedMemory::print() 
{
    if (sem_wait(mutex_sem_) == -1) 
        throw std::runtime_error("Mutex semaphore wait failed during print");

    try 
    {
        validate();

        size_t front = data_->front_.load(std::memory_order_relaxed);
        size_t rear = data_->rear_.load(std::memory_order_relaxed);
        size_t count = data_->count_.load(std::memory_order_relaxed);

        if (count == 0) 
            std::cout << "  [Empty]" << std::endl;
        else 
        {
            size_t index = front;
            for (size_t i = 0; i < count; ++i) 
            {
                const SharedTask& task = data_->tasks_[index];
                std::cout << "  Task ID: " << task.id_
                          << ", Priority: " << task.priority_
                          << ", Description: " << task.description_
                          << ", Completed: " << (task.completed_ ? "Yes" : "No")
                          << ", Remaining Time: " << task.remaining_time_ms_ << " ms"
                          << std::endl;
                index = (index + 1) % capacity_;
            }
        }
    } 
    catch (const std::exception& e) 
    {
        logger_error_->log("Error during print: " + std::string(e.what()));
        sem_post(mutex_sem_);
        throw;
    }
    
    sem_post(mutex_sem_);
}

SharedTask PosixSharedMemory::dequeue() 
{
    if (sem_wait(dequeue_sem_) == -1)
        throw std::runtime_error("Dequeue semaphore wait failed");

    if (sem_wait(mutex_sem_) == -1) 
    {
        sem_post(dequeue_sem_);
        throw std::runtime_error("Mutex semaphore wait failed");
    }

    size_t front = data_->front_.load(std::memory_order_relaxed);

    if (data_->count_.load(std::memory_order_relaxed) == 0) 
    {
        sem_post(mutex_sem_);
        sem_post(dequeue_sem_);
        throw std::runtime_error("Queue is empty");
    }

    SharedTask task = data_->tasks_[front];
    data_->front_.store((front + 1) % capacity_, std::memory_order_relaxed);
    data_->count_.fetch_sub(1, std::memory_order_relaxed);
    data_->total_dequeued_.fetch_add(1, std::memory_order_relaxed);

    logger_state_->log("Dequeued task with id: " + std::to_string(task.id_));

    sem_post(mutex_sem_);
    sem_post(enqueue_sem_);

    return task;
}

void PosixSharedMemory::cleanup() 
{
    try 
    {
        destroy();
    } 
    catch (const std::exception& e) 
    {
        logger_error_->log("Failed to clean up shared memory: " + std::string(e.what()));
    }
    catch (...) 
    {
        logger_error_->log("Unknown exception occurred during cleanup.");
    }
}

void PosixSharedMemory::validate() const 
{
    if (!data_) 
        throw std::runtime_error("Shared memory not attached");
}
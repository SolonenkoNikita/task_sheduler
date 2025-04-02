#include "PosixSharedMemory/PosixSharedMemory.hpp"

PosixSharedMemory::PosixSharedMemory(const std::string& name, size_t capacity)
        : name_(name), capacity_(capacity), fd_(-1), data_(nullptr) 
    {
        logger_ = std::make_shared<ErrorLogger>(LOGS_DIR, ERROR_DIR);
    }

PosixSharedMemory::~PosixSharedMemory() 
{
    cleanup();
}

void PosixSharedMemory::create() 
{
    std::lock_guard<std::mutex> lock(mutex_);

    fd_ = shm_open(name_.c_str(), O_CREAT | O_RDWR | O_EXCL, 0666);
    if (fd_ == -1) 
        throw std::runtime_error("Failed to create shared memory: " + std::string(strerror(errno)));

    if (ftruncate(fd_, sizeof(SharedMemoryLayout)) == -1) 
    {
        close(fd_);
        throw std::runtime_error("Ftruncate failed: " + std::string(strerror(errno)));
    }

    attach();
    memset(data_, 0, sizeof(SharedMemoryLayout));
}

void PosixSharedMemory::attach() 
{
    if (fd_ == -1) 
    {
        fd_ = shm_open(name_.c_str(), O_RDWR, 0666);
        if (fd_ == -1) 
        throw std::runtime_error("shm_open failed: " + std::string(strerror(errno)));
    }

    data_ = static_cast<SharedMemoryLayout *>(mmap(nullptr, sizeof(SharedMemoryLayout),
                    PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0));

    if (data_ == MAP_FAILED) 
        throw std::runtime_error("mmap failed: " + std::string(strerror(errno)));
}

void PosixSharedMemory::detach() 
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (data_) 
    {
        munmap(data_, sizeof(SharedMemoryLayout));
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
}

void PosixSharedMemory::enqueue(const SharedTask& task) 
{
    std::lock_guard<std::mutex> lock(mutex_);
    validate();

    if (data_->count_ >= capacity_) 
        throw std::runtime_error("Queue is full");

    data_->tasks_[data_->rear_] = task;
    data_->rear_ = (data_->rear_ + 1) % capacity_;
    ++data_->count_;
    ++data_->total_enqueued_;
}

SharedTask PosixSharedMemory::dequeue() 
{
    std::lock_guard<std::mutex> lock(mutex_);
    validate();

    if (data_->count_ == 0) 
        throw std::runtime_error("Queue is empty");

    SharedTask task = data_->tasks_[data_->front_];
    data_->front_ = (data_->front_ + 1) % capacity_;
    --data_->count_;
    ++data_->total_dequeued_;

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
        logger_->log("Failed to clean up shared memory: " + std::string(e.what()));
    }
    catch (...) 
    {
        logger_->log("Unknown exception occurred during cleanup.");
    }
}

void PosixSharedMemory::validate() const 
{
    if (!data_) 
        throw std::runtime_error("Shared memory not attached");
}

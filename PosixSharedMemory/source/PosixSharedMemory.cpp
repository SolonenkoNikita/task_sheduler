#include "PosixSharedMemory/PosixSharedMemory.hpp"

PosixSharedMemory::PosixSharedMemory(const std::string& name, size_t capacity)
        : name_(name), capacity_(capacity), fd_(-1), data_(nullptr),
        enqueue_sem_(SEM_FAILED), dequeue_sem_(SEM_FAILED), mutex_sem_(SEM_FAILED)
    {
        if(capacity == 0 || capacity > THROW_VALUE)
            throw std::invalid_argument("Invalid value of capacity");
        logger_ = std::make_shared<ErrorLogger>(LOGS_DIR, ERROR_DIR);
    }

PosixSharedMemory::~PosixSharedMemory() 
{
    cleanup();
}

void PosixSharedMemory::create() 
{
    size_t total_size = sizeof(SharedMemoryLayout) + (capacity_ - 1) * sizeof(SharedTask);

    fd_ = shm_open(name_.c_str(), O_CREAT | O_RDWR | O_EXCL, 0666);
    if (fd_ == -1) 
        throw std::runtime_error("Failed to create shared memory: " + std::string(strerror(errno)));

    if (ftruncate(fd_, sizeof(SharedMemoryLayout)) == -1) 
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

    enqueue_sem_ = sem_open(std::string("/" + name_ + "_enq").c_str(), 
            O_CREAT | O_EXCL, 0666, capacity_);
    dequeue_sem_ = sem_open(std::string("/" + name_ + "_deq").c_str(), 
            O_CREAT | O_EXCL,  0666, capacity_);
    mutex_sem_ = sem_open(std::string("/" + name_ + "_mut").c_str(),
            O_CREAT | O_EXCL, 0666, capacity_);
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

    sem_post(mutex_sem_);
    sem_post(dequeue_sem_);
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

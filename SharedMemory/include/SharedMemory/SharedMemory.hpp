#pragma once

#include <Task/Task.hpp>

#include <vector>

#define MAX_PATH 256

struct SharedTask 
{
    int id_;
    int priority_;
    char description_[MAX_PATH];
    bool completed_;
    int remaining_time_ms_;
};

class SharedMemory
{
public:
public:
    virtual ~SharedMemory() = default;

    virtual void create() = 0;
    virtual void attach() = 0;
    virtual void detach() = 0;
    virtual void destroy() = 0;

    virtual void enqueue(const SharedTask& ) = 0;
    virtual SharedTask dequeue() = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;

    virtual const std::string& name() const noexcept = 0;
    virtual size_t capacity() const noexcept = 0;
};

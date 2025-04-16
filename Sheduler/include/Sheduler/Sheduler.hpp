#pragma once

#include <Logger/Logger.hpp>
#include <SharedMemory/SharedMemory.hpp>
#include <Task/Task.hpp>

#include <algorithm>
#include <chrono>
#include <memory>
#include <vector>

class Sheduler final
{
public:
    enum class SchedulingAlgorithm 
    {
        ROUND_ROBIN,
        PRIORITY,
        COMPLETION_TIME,
        DYNAMIC_PRIORITY
    };
private:
    
private:
    std::shared_ptr<SharedMemory> shared_memory_;
    SchedulingAlgorithm algorithm_;
    std::chrono::milliseconds time_slice_;
    std::shared_ptr<Logger> logger_;
};
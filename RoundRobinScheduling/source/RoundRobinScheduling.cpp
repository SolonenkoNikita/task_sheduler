#include "RoundRobinScheduling/RoundRobingScheduling.hpp"

size_t RoundRobinScheduling::select_next_task(const std::vector<std::shared_ptr<GeneralTask>>& tasks)
{
    if (tasks.empty())
        throw std::runtime_error("No tasks available");

    current_index_ = (current_index_ + 1) % tasks.size();
    return current_index_;
}
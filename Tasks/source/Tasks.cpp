#include "Tasks/Tasks.hpp"

CpuIntensiveTask::CpuIntensiveTask(int id, std::chrono::milliseconds duration): UnixTask(id, "CPU-Intensive Task"), 
                    total_work_(duration), remaining_work_(duration)
{
    is_io_bound_ = false;
}

bool CpuIntensiveTask::execute(std::chrono::milliseconds quantum) 
{
    set_state(TaskState::RUNNING);
    auto start = std::chrono::steady_clock::now();

    double result = 0;
    for (size_t i = 0; i < 500000 * (quantum.count() / 10); ++i) 
    {
        result += std::sin(i) * std::cos(i);

        auto current_time = std::chrono::steady_clock::now();
        if (current_time - start >= quantum) 
        {
            auto actual_work = current_time - start;
            remaining_work_ -=
                std::chrono::duration_cast<std::chrono::milliseconds>(actual_work);
            cpu_usage_ = std::min(1.0f, static_cast<float>(actual_work.count()) / quantum.count());
            set_state(TaskState::READY);
            return false;               
        }
    }

    auto actual_work = std::chrono::steady_clock::now() - start;
    remaining_work_ -= std::chrono::duration_cast<std::chrono::milliseconds>(actual_work);
    
    cpu_usage_ = std::min(1.0f, static_cast<float>(actual_work.count()) / quantum.count());

    bool completed = remaining_work_ <= std::chrono::milliseconds::zero();
    set_state(completed ? TaskState::COMPLETED : TaskState::READY);
    return completed;
}

IoBoundTask::IoBoundTask(int id, const std::string &file_path, int operations): UnixTask(id, "I/O-Bound Task: " + file_path), 
                            file_path_(file_path), operations_remaining_(operations) 
{
    is_io_bound_ = true;
}

bool IoBoundTask::execute(std::chrono::milliseconds quantum) 
{
    set_state(TaskState::RUNNING);
    auto start = std::chrono::steady_clock::now();

    std::ofstream tmp(file_path_, std::ios::app);
    if (!tmp) 
    {
        set_state(TaskState::COMPLETED);
        return true; 
    }

    tmp << "Operation #" << operations_remaining_ << "\n";

    auto sleep_time = quantum / 2;
    auto end_time = start + sleep_time;
    while (std::chrono::steady_clock::now() < end_time)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    auto actual_work = std::chrono::steady_clock::now() - start;
    --operations_remaining_;
    cpu_usage_ = std::min(0.3f, static_cast<float>(actual_work.count()) / quantum.count());

    bool completed = operations_remaining_ <= 0;
    set_state(completed ? TaskState::COMPLETED : TaskState::READY);
    return completed;
}
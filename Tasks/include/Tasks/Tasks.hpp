#pragma once

#include <Task/Task.hpp>

#include <thread>
#include <filesystem>
#include <fstream>

class CpuIntensiveTask : public UnixTask
{
public:
    CpuIntensiveTask(int, std::chrono::milliseconds);

    bool execute(std::chrono::milliseconds) override;

private:
    std::chrono::milliseconds total_work_;
    std::chrono::milliseconds remaining_work_;
};

class IoBoundTask : public UnixTask 
{
public:
    IoBoundTask(int, const std::string&, int);

    bool execute(std::chrono::milliseconds quantum) override;

private:
    std::filesystem::path file_path_;
    int operations_remaining_;
};
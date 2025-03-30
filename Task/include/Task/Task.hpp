#pragma once

#include <any>
#include <chrono>
#include <iostream>
#include <string>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <cmath>
#include <algorithm>

class GeneralTask
{
public:
    virtual bool execute(std::chrono::milliseconds) = 0;

    virtual int get_priority() const noexcept = 0;

    virtual void set_static_priority(int) = 0;

    virtual void adjust_dynamic_priority() = 0;

    virtual const std::string& get_description() const noexcept = 0;

    virtual std::string& get_description() noexcept = 0;

    virtual std::chrono::steady_clock::time_point get_arrival_time() const noexcept = 0;

    virtual bool is_completed() const noexcept = 0;

    virtual std::any get_attribute(const std::string&) const noexcept = 0;

    virtual void set_attribute(const std::string&, const std::any&) = 0;

    virtual ~GeneralTask() = default;
};

class UnixTask : public GeneralTask
{
public:
    enum class TaskState { READY, RUNNING, WAITING, COMPLETED };

    UnixTask(int, std::string, int = 0);

    [[nodiscard]] inline int get_id() const noexcept
    {
        return id_;
    }

    [[nodiscard]] inline int get_priority() const noexcept override 
    {
        return dynamic_priority_;
    }

    [[nodiscard]] inline const std::string& get_description() const noexcept override 
    { 
        return description_; 
    }

    [[nodiscard]] inline std::string& get_description() noexcept override 
    { 
        return description_; 
    }
    
    [[nodiscard]] inline bool is_completed() const noexcept override 
    { 
        return completed_; 
    }

    [[nodiscard]] inline std::chrono::steady_clock::time_point get_arrival_time() const noexcept override 
    {
        return arrival_time_;
    }
    
    [[nodiscard]] std::any get_attribute(const std::string &name) const noexcept override;

    void set_attribute(const std::string& name, const std::any& value) override;

    void set_state(TaskState state);

    void set_static_priority(int) override;

    [[nodiscard]] inline TaskState get_state() const noexcept 
    { 
        return state_; 
    }

    void adjust_dynamic_priority() override;

    bool execute(std::chrono::milliseconds) override;

    [[nodiscard]] pid_t launch_process(const std::string&);

protected:
    inline void validate_priority() const 
    {
        if (static_priority_ < -20 || static_priority_ > 19) 
            throw std::invalid_argument("Priority must be in [-20, 19]");
    }

    [[nodiscard]] bool check_process_status() const noexcept;

protected:
    const int id_;

    std::string description_;

    std::chrono::steady_clock::time_point arrival_time_;

    [[maybe_unused]] std::unordered_map<std::string, std::any> attributes_;

    int static_priority_ = 0; 

    int dynamic_priority_ = 0;
    
    TaskState state_ = TaskState::READY;

    bool completed_ = false;

    [[maybe_unused]] pid_t pid_ = -1;

    std::chrono::steady_clock::time_point last_execution_time_;
    float cpu_usage_ = 0.0f; 
    float virtual_runtime_ = 0.0f;
    bool is_io_bound_ = false;

    struct SchedulingParams 
    {
        float cpu_weight = 0.7f;
        float starvation_weight = 0.3f;
        float io_boost = 0.2f;
    };

    SchedulingParams scheduling_params_;
};

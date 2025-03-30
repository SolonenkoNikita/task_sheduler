#include "Task/Task.hpp"

UnixTask::UnixTask(int id, std::string desc, int static_prio) : id_(id), description_(std::move(desc)),
                            arrival_time_(std::chrono::steady_clock::now()),
                            static_priority_(static_prio), dynamic_priority_(static_prio) {
    validate_priority();
}

[[nodiscard]] std::any UnixTask::get_attribute(const std::string &name) const noexcept 
{
    auto it = attributes_.find(name);
    if (it != attributes_.cend()) 
        return it->second;
    return std::any{};
}

void UnixTask::set_attribute(const std::string &name, const std::any &value)
{
    attributes_[name] = value;
}

void UnixTask::set_state(TaskState state) 
{ 
    if(state == TaskState::COMPLETED)
        completed_ = true;
    state_ = state; 
}

[[nodiscard]] bool UnixTask::check_process_status() const noexcept 
{
    if (pid_ == -1)
        return true;
    int status;
    return waitpid(pid_, &status, WNOHANG) > 0;
}

void UnixTask::set_static_priority(int nice_value) 
{
    static_priority_ = nice_value;
    validate_priority();
    adjust_dynamic_priority();
}

[[nodiscard]] pid_t UnixTask::launch_process(const std::string &command) 
{
    pid_ = fork();
    if (pid_ == 0) 
    {
        setpriority(PRIO_PROCESS, 0, static_priority_);
        execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);
        exit(EXIT_FAILURE);
    } 
    else if (pid_ < 0) 
        throw std::runtime_error("Failed to fork process");
    return pid_;
}

bool UnixTask::execute(std::chrono::milliseconds) 
{
    throw std::runtime_error("Execute method not implemented");
}

void UnixTask::adjust_dynamic_priority()
{
    const auto now = std::chrono::steady_clock::now();
    const auto time_since_last_run = now - last_execution_time_;

    float starvation_factor = std::clamp(std::chrono::duration<float>(time_since_last_run).count() / 10.0f, 0.0f,
        1.0f);

    float priority_adjustment = (scheduling_params_.cpu_weight * cpu_usage_) -
                            (scheduling_params_.starvation_weight * starvation_factor);

    if (is_io_bound_) 
        priority_adjustment -= scheduling_params_.io_boost;

    dynamic_priority_ = static_cast<int>(static_priority_ + (priority_adjustment * 20));

    dynamic_priority_ = std::clamp(dynamic_priority_, -20, 19);
    last_execution_time_ = now;
    cpu_usage_ = 0.0f;
}
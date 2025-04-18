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
#include <memory>

#include <Logger/Logger.hpp>

#define STATE_DIR "state_log"

/**
 * @class GeneralTask
 * @brief Abstract base class representing a generic task.
 *
 * Defines the interface for tasks, including execution, priority management, state tracking, and attribute handling.
 */
class GeneralTask
{
public:

    /**
     * @brief Executes the task for a given duration.
     *
     * @param duration The duration for which the task should execute.
     * @return bool True if the task was executed successfully, false otherwise.
     */
    virtual bool execute(std::chrono::milliseconds) = 0;

    /**
     * @brief Retrieves the task's priority.
     *
     * @return int The task's priority.
     */
    virtual int get_priority() const noexcept = 0;

    /**
     * @brief Sets the static priority of the task.
     *
     * @param priority The new static priority value.
     */
    virtual void set_static_priority(int) = 0;

    /**
     * @brief Adjusts the dynamic priority of the task.
     */
    virtual void adjust_dynamic_priority() = 0;

    /**
     * @brief Retrieves the task's description (const version).
     *
     * @return const std::string& The task's description.
     */
    virtual const std::string& get_description() const noexcept = 0;

    /**
     * @brief Retrieves the task's description (non-const version).
     *
     * @return std::string& The task's description.
     */
    virtual std::string& get_description() noexcept = 0;

    /**
     * @brief Retrieves the task's arrival time.
     *
     * @return std::chrono::steady_clock::time_point The time when the task arrived.
     */
    virtual std::chrono::steady_clock::time_point get_arrival_time() const noexcept = 0;

    /**
     * @brief Checks if the task is completed.
     *
     * @return bool True if the task is completed, false otherwise.
     */
    virtual bool is_completed() const noexcept = 0;

    /**
     * @brief Retrieves an attribute by name.
     *
     * @param name The name of the attribute.
     * 
     * @return std::any The attribute value, or an empty `std::any` if not found.
     * std::any is a class for storing any value
     */
    virtual std::any get_attribute(const std::string&) const noexcept = 0;

    /**
     * @brief Sets an attribute by name.
     *
     * @param name The name of the attribute.
     * @param value The value to set for the attribute.
     */
    virtual void set_attribute(const std::string&, const std::any&) = 0;

    /**
     * @brief Retrieves the total time required for the task to complete.
     *
     * @return std::chrono::milliseconds The total duration of the task.
     */
    virtual std::chrono::milliseconds get_total_time() const noexcept = 0;

    virtual int get_id() const noexcept = 0;

    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~GeneralTask() = default;
};

/**
 * @class UnixTask
 * @brief Represents a task in a Unix-based system.
 *
 * Implements the `GeneralTask` interface and provides additional functionality
 * for managing task states, priorities, and attributes.
 */
class UnixTask : public GeneralTask
{
public:

    /**
     * @enum TaskState
     * @brief Represents the state of a task.
     *
     * Possible states include READY, RUNNING, WAITING, and COMPLETED.
     */
    enum class TaskState { READY, RUNNING, WAITING, COMPLETED };

    /**
     * @brief Constructs a `UnixTask` instance.
     *
     * @param id The unique identifier for the task.
     * @param desc A description of the task.
     * @param static_prio The initial static priority of the task (default: 0).
     */
    UnixTask(int, std::string, int = 0);

    /**
     * @brief Retrieves the task's ID.
     *
     * @return int The task's ID.
     */
    [[nodiscard]] inline int get_id() const noexcept override
    {
        return id_;
    }

    /**
     * @brief Retrieves the task's priority.
     *
     * @return int The task's current priority.
     */
    [[nodiscard]] inline int get_priority() const noexcept override 
    {
        return dynamic_priority_;
    }

    /**
     * @brief Retrieves the task's description (const version).
     *
     * @return const std::string& The task's description.
     */
    [[nodiscard]] inline const std::string& get_description() const noexcept override 
    { 
        return description_; 
    }

    /**
     * @brief Retrieves the task's description (non-const version).
     *
     * @return std::string& The task's description.
     */
    [[nodiscard]] inline std::string& get_description() noexcept override 
    { 
        return description_; 
    }

    /**
     * @brief Checks if the task is completed.
     *
     * @return bool True if the task is completed, false otherwise.
     */
    [[nodiscard]] inline bool is_completed() const noexcept override 
    { 
        return completed_; 
    }

    /**
     * @brief Retrieves the task's arrival time.
     *
     * @return std::chrono::steady_clock::time_point The time when the task arrived.
     */
    [[nodiscard]] inline std::chrono::steady_clock::time_point get_arrival_time() const noexcept override 
    {
        return arrival_time_;
    }

    /**
     * @brief Retrieves an attribute by name.
     *
     * @param name The name of the attribute.
     * @return std::any The attribute value, or an empty `std::any` if not found.
     */
    [[nodiscard]] std::any get_attribute(const std::string &name) const noexcept override;

    /**
     * @brief Sets an attribute by name.
     *
     * @param name The name of the attribute.
     * @param value The value to set for the attribute.
     */
    void set_attribute(const std::string& name, const std::any& value) override;

    /**
     * @brief Sets the task's state.
     *
     * Logs the state change and updates the task's state.
     *
     * @param state The new state of the task.
     */
    void set_state(TaskState state);

    /**
     * @brief Sets the static priority of the task.
     *
     * Validates the priority and adjusts the dynamic priority accordingly.
     *
     * @param nice_value The new static priority value.
     */
    void set_static_priority(int) override;

    /**
     * @brief Retrieves the task's current state.
     *
     * @return TaskState The current state of the task.
     */
    [[nodiscard]] inline TaskState get_state() const noexcept 
    { 
        return state_; 
    }

    /**
     * @brief Adjusts the dynamic priority of the task.
     *
     * Calculates the dynamic priority based on CPU usage, starvation factor,
     * and I/O bound status.
     */
    void adjust_dynamic_priority() override;

    /**
     * @brief Executes the task for a given duration.
     *
     * Throws an exception as this method is not implemented.
     *
     * @param duration The duration for which the task should execute.
     * @return bool Always throws an exception.
     */
    bool execute(std::chrono::milliseconds) override;

    /**
     * @brief Launches a process for the task.
     *
     * Forks a new process and executes the specified command.
     *
     * @param command The command to execute in the new process.
     * @return pid_t The PID of the launched process.
     * @throws std::runtime_error If the process cannot be forked.
     */
    [[nodiscard]] pid_t launch_process(const std::string&);
private:

     /**
     * @brief Converts a `TaskState` enum to its string representation.
     *
     * @param state The task state to convert.
     * @return std::string The string representation of the task state.
     */
    std::string task_state_to_string(TaskState);

protected:

    /**
     * @brief Validates the task's priority.
     *
     * Ensures the priority is within the valid range [-20, 19].
     *
     * @throws std::invalid_argument If the priority is out of range.
     */
    inline void validate_priority() const 
    {
        if (static_priority_ < -20 || static_priority_ > 19) 
            throw std::invalid_argument("Priority must be in [-20, 19]");
    }

    /**
     * @brief Checks the status of the task's associated process.
     *
     * @return bool True if the process has terminated, false otherwise.
     */
    [[nodiscard]] bool check_process_status() const noexcept;

    [[maybe_unused]] inline std::chrono::milliseconds get_total_time() const noexcept override {}

protected:
    const int id_;

    std::string description_;

    std::chrono::steady_clock::time_point arrival_time_;

    [[maybe_unused]] std::unordered_map<std::string, std::any> attributes_;

    int static_priority_ = 0; 

    int dynamic_priority_ = 0;
    
    TaskState state_ = TaskState::READY;

    bool completed_ = false;

    [[maybe_unused]] pid_t pid_ = -1; ///< PID of the associated process

    std::chrono::steady_clock::time_point last_execution_time_;
    float cpu_usage_ = 0.0f;        ///< CPU usage of the task.
    float virtual_runtime_ = 0.0f; ///< Virtual runtime of the task
    bool is_io_bound_ = false;    ///< Indicates whether the task is I/O bound.

    /**
     * @struct SchedulingParams
     * @brief Parameters used for dynamic priority adjustment.
     */
    struct SchedulingParams 
    {
        float cpu_weight = 0.7f;
        float starvation_weight = 0.3f;
        float io_boost = 0.2f;
    };

    SchedulingParams scheduling_params_;

    std::shared_ptr<Logger> logger_;
};

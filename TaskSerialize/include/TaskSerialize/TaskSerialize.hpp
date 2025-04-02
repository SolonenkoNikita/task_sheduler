#pragma once

class UnixTask;
class CpuIntensiveTask;
class IoBoundTask;

class TaskSerializer 
{
public:
    virtual ~TaskSerializer() = default;

    virtual SharedTask serialize(const UnixTask&) const = 0;
    virtual SharedTask serialize(const CpuIntensiveTask&) const = 0;
    virtual SharedTask serialize(const IoBoundTask&) const = 0;
};
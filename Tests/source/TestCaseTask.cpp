#include <Task/Task.hpp>

#include <gtest/gtest.h>

TEST(MethodsTestTask, PriorityValidation) 
{
    UnixTask task(1, "Test case", 10);

    ASSERT_EQ(task.get_priority(), 10);
}

TEST(MethodsTestTask, GetIdBasicUnixTask)
{
    for(size_t i = 0; i < 100; ++i)
    {
        UnixTask* task = new UnixTask(i, "Test case");
        ASSERT_EQ(i, task->get_id());
        delete task;
    }
}

TEST(MethodsTestTask, LaunchProcesseWithBacisUnixTask) 
{
    UnixTask task(1, "Test case");

    pid_t pid = task.launch_process("echo 'Hello, World!'");

    ASSERT_GT(pid, 0);

    int status;
    ASSERT_NE(waitpid(pid, &status, WNOHANG), -1); 
}

TEST(MethodsTestTask, ExceptionExeWithBacisUnixTask1)
{
    ASSERT_THROW( GeneralTask *task = new UnixTask(1, "Test case", 22);, std::invalid_argument);
}

TEST(MethodsTestTask, ExceptionExeWithBacisUnixTask2)
{
    ASSERT_THROW( GeneralTask *task = new UnixTask(1, "Test case", -21);, std::invalid_argument);
}

TEST(MethodsTestTask, ExceptionExeWithBacisUniPxTask3) 
{
    GeneralTask* task = new UnixTask(1, "Test case");
    ASSERT_THROW(task->execute(std::chrono::milliseconds(0)), std::runtime_error);
}

TEST(MethodsTestTask, ArrivalTimeWithBasicUnixTask) 
{
    auto now = std::chrono::steady_clock::now();
    UnixTask task(1, "Test case");

    auto arrival_time = task.get_arrival_time();
    ASSERT_GE(arrival_time, now - std::chrono::milliseconds( 10)); 
    ASSERT_LE(arrival_time, now + std::chrono::milliseconds(10));
}

TEST(MethodsTestTask, ExecuteThrowsExceptionWithBasicUnixTask) 
{
    UnixTask task(1, "Test case");
    ASSERT_THROW(task.execute(std::chrono::milliseconds(100)), std::runtime_error);
}

TEST(MethodsTestTask, DescriptionWithBasicUnixTask) 
{
    UnixTask task(1, "Initial description");

    ASSERT_EQ(task.get_description(), "Initial description");

    task.get_description() = "Updated description";
    ASSERT_EQ(task.get_description(), "Updated description");
}

TEST(MethodsTestTask, AttributesWithBasicUnixTask) 
{
    UnixTask task(1, "Test case");

    task.set_attribute("test_key", std::string("test_value"));

    auto value = task.get_attribute("test_key");
    ASSERT_TRUE(value.has_value());
    ASSERT_EQ(std::any_cast<std::string>(value), "test_value");

    auto missing_value = task.get_attribute("nonexistent_key");
    ASSERT_FALSE(missing_value.has_value());
}

TEST(MethodsTestTask, TaskStateWithBasicUnixTask) 
{
    UnixTask task(1, "Test case");

    ASSERT_EQ(task.get_state(), UnixTask::TaskState::READY);

    task.set_state(UnixTask::TaskState::RUNNING);
    ASSERT_EQ(task.get_state(), UnixTask::TaskState::RUNNING);

    task.set_state(UnixTask::TaskState::COMPLETED);
    ASSERT_TRUE(task.is_completed());

    task.set_state(UnixTask::TaskState::WAITING);
    ASSERT_EQ(task.get_state(), UnixTask::TaskState::WAITING);
}
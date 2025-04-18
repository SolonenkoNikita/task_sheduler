#include <gtest/gtest.h>

#include <PosixSharedMemory/PosixSharedMemory.hpp>
#include <TaskQueueManager/TaskQueueManager.hpp>
#include <Tasks/Tasks.hpp>
#include <RoundRobinScheduling/RoundRobingScheduling.hpp>
#include <PriorityScheduling/PriorityScheduling.hpp>

class TaskQueueManagerTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {   
        shared_memory_ = std::make_shared<PosixSharedMemory>("/test_shared_memory", 100);
        try 
        {
            shared_memory_->create();
        }  
        catch (const std::exception& e) 
        {
            std::cerr << "Exception caught during create(): " << e.what() << std::endl;
            shared_memory_->attach();
        }
        queue_manager_ = std::make_unique<TaskQueueManager>(shared_memory_);
    }
    std::shared_ptr<PosixSharedMemory> shared_memory_;
    std::unique_ptr<TaskQueueManager> queue_manager_;
};

TEST_F(TaskQueueManagerTest, AddTask) 
{
    auto task = std::make_shared<UnixTask>(1, "Test Task");

    queue_manager_->add_task(task);

    EXPECT_EQ(queue_manager_->task_count(), 1);

    auto task1 = std::make_shared<IoBoundTask>(3, "Test Task", 10);
    queue_manager_->add_task(task1);

    EXPECT_EQ(queue_manager_->task_count(), 2);
    auto task2 = std::make_shared<CpuIntensiveTask>(2, std::chrono::seconds(5));
    queue_manager_->add_task(task2);

    EXPECT_EQ(queue_manager_->task_count(), 3);
}

TEST_F(TaskQueueManagerTest, GetNextTask) 
{
    auto task = std::make_shared<UnixTask>(1, "Test Task");
    queue_manager_->add_task(task);

    auto retrieved_task = queue_manager_->get_next_task();

    ASSERT_NE(retrieved_task, nullptr);
    EXPECT_EQ(retrieved_task->get_id(), 1);
    EXPECT_EQ(retrieved_task->get_description(), "Test Task");
}

TEST_F(TaskQueueManagerTest, ReorderTasksWithPriorityScheduling) 
{
    auto task1 = std::make_shared<UnixTask>(1, "Task 1");
    auto task2 = std::make_shared<UnixTask>(2, "Task 2");

    task1->set_static_priority(10); 
    task2->set_static_priority(5);

    queue_manager_->add_task(task1);
    queue_manager_->add_task(task2);

    auto algorithm = std::make_shared<PriorityScheduling>();
    queue_manager_->reorder_tasks(algorithm);
    
    EXPECT_EQ(queue_manager_->task_count(), 2);

    auto next_task = queue_manager_->get_next_task();
    ASSERT_NE(next_task, nullptr);
    EXPECT_EQ(next_task->get_id(), 1);
}

TEST_F(TaskQueueManagerTest, ReorderTasksWithRoundRobinScheduling) 
{
    auto task1 = std::make_shared<UnixTask>(1, "Task 1");
    auto task2 = std::make_shared<UnixTask>(2, "Task 2");

    queue_manager_->add_task(task1);
    queue_manager_->add_task(task2);

    auto algorithm = std::make_shared<RoundRobinScheduling>();

    queue_manager_->reorder_tasks(algorithm);
    auto next_task = queue_manager_->get_next_task();
    ASSERT_NE(next_task, nullptr);
    EXPECT_EQ(next_task->get_id(), 1); // Task 1 должна быть выбрана первой

    queue_manager_->reorder_tasks(algorithm);
    next_task = queue_manager_->get_next_task();
    ASSERT_NE(next_task, nullptr);
    EXPECT_EQ(next_task->get_id(), 2); // Task 2 должна быть выбрана второй

}

TEST_F(TaskQueueManagerTest, TaskCount) 
{
    EXPECT_EQ(queue_manager_->task_count(), 0);

    auto task = std::make_shared<UnixTask>(1, "Test Task");
    queue_manager_->add_task(task);

    EXPECT_EQ(queue_manager_->task_count(), 1);
}

TEST_F(TaskQueueManagerTest, ReorderTasksWithEmptyQueueUsingRoundRobin) 
{
    auto algorithm = std::make_shared<RoundRobinScheduling>();

    queue_manager_->reorder_tasks(algorithm);

    EXPECT_EQ(queue_manager_->task_count(), 0);
}

TEST_F(TaskQueueManagerTest, ReorderTasksWithEmptyQueueUsingPriorityScheduling) 
{
    auto algorithm = std::make_shared<PriorityScheduling>();

    queue_manager_->reorder_tasks(algorithm);

    EXPECT_EQ(queue_manager_->task_count(), 0);
}
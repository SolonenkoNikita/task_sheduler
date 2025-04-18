#include <PosixSharedMemory/PosixSharedMemory.hpp>
#include <TaskProcessor/TaskProcessor.hpp>
#include <TaskQueueManager/TaskQueueManager.hpp>
#include <Tasks/Tasks.hpp>

#include <gtest/gtest.h>

class TaskProcessorTest : public ::testing::Test 
{
protected:

    void SetUp() override 
    {
        shared_memory_ = std::make_shared<PosixSharedMemory>("/test_shared_memory", 100);
        try 
        {
            shared_memory_->create();
        } 
        catch (...) 
        {
            shared_memory_->attach();
        }
        queue_manager_ = std::make_shared<TaskQueueManager>(shared_memory_);
        processor_ = std::make_unique<TaskProcessor>(queue_manager_, std::chrono::milliseconds(500));
    }

    void TearDown() override 
    {
        if (processor_) 
            processor_->stop();
    }

    std::shared_ptr<PosixSharedMemory> shared_memory_;
    std::shared_ptr<TaskQueueManager> queue_manager_;
    std::unique_ptr<TaskProcessor> processor_;
};

TEST_F(TaskProcessorTest, StartAndStop) 
{
 
    processor_->start();
    EXPECT_TRUE(processor_->is_running()); 

    processor_->stop();
    EXPECT_FALSE(processor_->is_running());
}

TEST_F(TaskProcessorTest, ProcessSingleTask) 
{
    auto task = std::make_shared<CpuIntensiveTask>(1, std::chrono::milliseconds(600));
    queue_manager_->add_task(task);

    processor_->start();

    processor_->stop();

    EXPECT_EQ(queue_manager_->task_count(), 1);
}

TEST_F(TaskProcessorTest, ReenqueueIncompleteTask) 
{
    auto task = std::make_shared<CpuIntensiveTask>(1, std::chrono::milliseconds(50));
    queue_manager_->add_task(task);

    processor_->start();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    EXPECT_EQ(queue_manager_->task_count(), 0);

    processor_->stop();
}
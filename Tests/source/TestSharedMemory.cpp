#include <PosixSharedMemory/PosixSharedMemory.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

class PosixSharedMemoryTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        sem_unlink("/test_shm_enq");
        sem_unlink("/test_shm_deq");
        sem_unlink("/test_shm_mut");
        shm_unlink("/test_shm");
    }

    void TearDown() override 
    {
        sem_unlink("/test_shm_enq");
        sem_unlink("/test_shm_deq");
        sem_unlink("/test_shm_mut");
        shm_unlink("/test_shm");
    }
};

TEST_F(PosixSharedMemoryTest, CreateAndDestroy) 
{
    {
        PosixSharedMemory shm("/test_shm", 10);
        EXPECT_NO_THROW(shm.create());
        EXPECT_FALSE(shm.is_scheduler_running());
        EXPECT_EQ(shm.size(), 0);
        EXPECT_TRUE(shm.empty());
    }
}

TEST_F(PosixSharedMemoryTest, CircularBufferBehavior) 
{
    PosixSharedMemory shm("/test_shm", 3);
    shm.create();

    SharedTask tasks[3] = {
        {1, 1, "Task1", TaskType::UNIX_TASK, false, 100},
        {2, 2, "Task2", TaskType::UNIX_TASK, false, 200},
        {3, 3, "Task3", TaskType::UNIX_TASK,false, 300}
    };

    for (auto& task : tasks) 
        shm.enqueue(task);
    EXPECT_EQ(shm.size(), 3);

    shm.dequeue();
    shm.dequeue();
    EXPECT_EQ(shm.size(), 1);

    SharedTask task4{4, 4, "Task4", TaskType::UNIX_TASK, false, 400};
    shm.enqueue(task4);
    EXPECT_EQ(shm.size(), 2);

    EXPECT_EQ(shm.dequeue().id_, 3);
    EXPECT_EQ(shm.dequeue().id_, 4);
}

TEST_F(PosixSharedMemoryTest, MultithreadedAccess) 
{
    PosixSharedMemory shm("/test_shm", 100);
    shm.create();

    const int num_threads = 4;
    const int tasks_per_thread = 25;
    std::vector<std::thread> enqueue_threads;
    std::vector<std::thread> dequeue_threads;

    for (int i = 0; i < num_threads; ++i)
    {
        enqueue_threads.emplace_back([&shm, i]() 
        {
            for (int j = 0; j < tasks_per_thread; ++j) 
            {
                SharedTask task{i * 100 + j, j, "Task", TaskType::UNIX_TASK, false, 100};
                shm.enqueue(task);
            }
        });
    }

    std::atomic<int> total_dequeued{0};
    for (int i = 0; i < num_threads; ++i) 
    {
        dequeue_threads.emplace_back([&shm, &total_dequeued]() 
        {
            for (int j = 0; j < tasks_per_thread; ) 
            {
                try 
                {
                    shm.dequeue();
                    ++total_dequeued;
                    ++j;
                } 
                catch (...) 
                {
                    std::this_thread::yield();
                }
            }
        });
    }

    for (auto& t : enqueue_threads) t.join();
    for (auto& t : dequeue_threads) t.join();

    EXPECT_EQ(total_dequeued.load(), num_threads * tasks_per_thread);
    EXPECT_TRUE(shm.empty());
}

TEST_F(PosixSharedMemoryTest, EnqueueDequeueSingleThread) 
{
    PosixSharedMemory shm("/test_shm", 5);
    shm.create();

    SharedTask task1{1, 10, "Task1", TaskType::UNIX_TASK, false, 100};
    SharedTask task2{2, 5, "Task2", TaskType::UNIX_TASK, false, 200};
    
    EXPECT_NO_THROW(shm.enqueue(task1));
    EXPECT_EQ(shm.size(), 1);
    EXPECT_FALSE(shm.empty());

    EXPECT_NO_THROW(shm.enqueue(task2));
    EXPECT_EQ(shm.size(), 2);

    auto dequeued = shm.dequeue();
    EXPECT_EQ(dequeued.id_, 1);
    EXPECT_EQ(shm.size(), 1);

    dequeued = shm.dequeue();
    EXPECT_EQ(dequeued.id_, 2);
    EXPECT_TRUE(shm.empty());
}

TEST_F(PosixSharedMemoryTest, SchedulerFlag) 
{
    PosixSharedMemory shm("/test_shm");
    shm.create();

    EXPECT_FALSE(shm.is_scheduler_running());
    
    shm.set_scheduler_running(true);
    EXPECT_TRUE(shm.is_scheduler_running());
    
    shm.set_scheduler_running(false);
    EXPECT_FALSE(shm.is_scheduler_running());
}

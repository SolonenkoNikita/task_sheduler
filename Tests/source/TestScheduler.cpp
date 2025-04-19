#include <gtest/gtest.h>

#include <Sheduler/Sheduler.hpp>

TEST(SchedulerTest, StartAndStop) 
{
    auto shm = std::make_shared<PosixSharedMemory>("/test_scheduler");
    try
    {
        shm->create();
    }
    catch (...) 
    {
        shm->attach();
    }
    Scheduler scheduler(shm);

    scheduler.start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    scheduler.stop();
}

TEST(SchedulerTest, AddTask) 
{
    auto shm = std::make_shared<PosixSharedMemory>("/test_scheduler");
    Scheduler scheduler(shm);
    try
    {
        shm->create();
    }
    catch (...) 
    {
        shm->attach();
    }

    scheduler.add_task(std::make_shared<CpuIntensiveTask>(1, std::chrono::milliseconds(5)));
    EXPECT_EQ(scheduler.get_count(), 1);

    scheduler.add_task(std::make_shared<CpuIntensiveTask>(2, std::chrono::milliseconds(15)));
    scheduler.add_task(std::make_shared<IoBoundTask>(2, "output.txt", 10));
    EXPECT_EQ(scheduler.get_count(), 3);

    scheduler.start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    scheduler.stop();
}

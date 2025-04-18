#include <Sheduler/Sheduler.hpp>
#include <Tasks/Tasks.hpp>

int main()
{
    auto shm = std::make_shared<PosixSharedMemory>("/task_queue");
    try 
    {
        shm->create();
    } 
    catch (...) 
    {
        shm->attach();
    }

    Scheduler scheduler(shm);

    scheduler.add_task(std::make_shared<CpuIntensiveTask>(1, std::chrono::seconds(5)));
    //scheduler.add_task(std::make_shared<IoBoundTask>(2, "output.txt", 10));

    scheduler.start();

    //std::this_thread::sleep_for(std::chrono::seconds(30));

   // scheduler.stop();
    return 0;
}
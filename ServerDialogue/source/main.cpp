#include <Sheduler/Sheduler.hpp>
#include <Tasks/Tasks.hpp>

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

#define PORT 8080

static const std::unordered_map<std::string, int> operations
{
    {"add", 19},
    {"sub", 18},
    {"mul", 15},
    {"del", 16}
};

void handle_client(int client_socket, Scheduler& scheduler) 
{
    char buffer[1024] = {0};
    read(client_socket, buffer, sizeof(buffer));
    std::string command(buffer);
    std::cout << "Received command: " << command << std::endl;

    std::istringstream iss(command);
    std::string operation;
    int num1, num2;

    if (!(iss >> operation >> num1 >> num2))
    {
        std::cerr << "Invalid command format\n";
        close(client_socket);
        return;
    }

    auto it = operations.find(operation);
    if (it != operations.cend())
    {
        auto task = std::make_shared<UnixTask>(it->second, operation);
        task->set_static_priority(it->second);
        scheduler.add_task(task);
        std::cout << "Task added: " << operation << " "  << num1 << " and "  << num2;
    }    
    else 
        std::cerr << "Unknown operation: " << operation << std::endl;

    close(client_socket);
}

void start_server(Scheduler& scheduler) 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
     {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on port " << PORT << std::endl;

    while (true) 
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        std::thread(handle_client, new_socket, std::ref(scheduler)).detach();
    }
}

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

    scheduler.start();

    scheduler.add_task(std::make_shared<CpuIntensiveTask>(1, std::chrono::seconds(1)));
    scheduler.add_task(std::make_shared<IoBoundTask>(2, "output.txt", 10));

    start_server(scheduler);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    scheduler.stop();
    return 0;
}
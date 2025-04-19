#include "Server/Server.hpp"

void Server::handle_client(int client_socket, Scheduler& scheduler) 
{
    char buffer[SIZE] = {0};
    read(client_socket, buffer, sizeof(buffer));
    std::string command(buffer);
    logger_normal_->log("Received command: " + command);

    std::istringstream iss(command);
    std::string operation;
    int num1, num2;

    if (!(iss >> operation >> num1 >> num2)) 
    {
        logger_->log("Invalid command format");
        close(client_socket);
        return;
    }
    auto it = operations.find(operation);
    if (it != operations.cend())
    {
        auto task = std::make_shared<UnixTask>(it->second, operation);
        task->set_static_priority(it->second);
        scheduler.add_task(task);
        logger_normal_->log("Task added: " + operation + " " + std::to_string(num1) + " and " + 
        std::to_string(num2));
   
    }    
    else
       logger_->log("Unknown operation: " + operation);

    close(client_socket);
}

void Server::start_server(Scheduler& scheduler) 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        logger_->log("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        logger_->log("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        logger_->log("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) 
    {
        logger_->log("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on port " <<  std::to_string(PORT);
    logger_normal_->log("Server started on port " + std::to_string(PORT));

    while (true) 
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) 
        {
            logger_->log("Accept failed");
            exit(EXIT_FAILURE);
        }
        std::thread([this, new_socket, &scheduler]() {
            this->handle_client(new_socket, scheduler);
        }).detach();
    }
}
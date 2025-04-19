#pragma once

#include <Sheduler/Sheduler.hpp>
#include <Tasks/Tasks.hpp>

#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

#define PORT 8080
#define SERVER "server_state"
#define SERVER_ERROR "server_error"

constexpr int SIZE = 1024;

static const std::unordered_map<std::string, int> operations
{
    {"add", 19},
    {"sub", 18},
    {"mul", 15},
    {"del", 16}
};

class Server final
{
public:
    explicit Server(): logger_(std::make_unique<ErrorLogger>(LOGS_DIR, SERVER_ERROR)),
        logger_normal_(std::make_unique<FileLogger>(LOGS_DIR, SERVER)) {}

    void handle_client(int, Scheduler&);

    void start_server(Scheduler&);

private:
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Logger> logger_normal_;
};
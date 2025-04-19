#pragma once 

#include <Logger/Logger.hpp>

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define CLIENT "client_state"
#define CLIENT_ERROR "client_error"

class Client final
{
public:
    explicit Client() : logger_(std::make_shared<ErrorLogger>(LOGS_DIR, CLIENT_ERROR)), 
        logger_normal_(std::make_shared<FileLogger>(LOGS_DIR, CLIENT)){}
    
    void send_command(const std::string&);
private:
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Logger> logger_normal_;
};
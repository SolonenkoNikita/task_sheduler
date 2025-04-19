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

/**
 * @class Client
 * @brief Represents a client that sends commands to a server.
 *
 * This class manages the creation of a TCP socket, connection to the server,
 * and sending commands. It logs errors and normal events using Logger
 * instances.
 */
class Client final
{
public:
    /**
     * @brief Constructs a Client instance.
     *
     * Initializes two loggers: one for error logging and one for normal event logging.
     */
    explicit Client() : logger_(std::make_shared<ErrorLogger>(LOGS_DIR, CLIENT_ERROR)), 
        logger_normal_(std::make_shared<FileLogger>(LOGS_DIR, CLIENT)){}

    /**
     * @brief Sends a command to the server.
     *
     * Creates a socket, connects to the server, and sends the specified
     * command. Logs errors if any step fails and logs the command if sent
     * successfully.
     *
     * @param command The command to send to the server.
     */
    void send_command(const std::string&);
private:
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Logger> logger_normal_;
};
#include "Client/Client.hpp"

void Client::send_command(const std::string& command) 
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        logger_->log("Socket creation error");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
    {
        logger_->log("Invalid address/ Address not supported");
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        logger_->log("Connection Failed");
        return;
    }

    send(sock, command.c_str(), command.size(), 0);
    logger_normal_->log("Command sent: " + command);

    close(sock);
}